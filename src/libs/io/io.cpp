//
// Created by travor on 20/08/2023.
//

#define _WINSOCKAPI_
#include "io.h"

#ifdef _WIN32
    #include <io.h>
#endif

#include <uv.h>

namespace bond::io {
#define ALLOC_ATOMIC(T) (T*)GC_malloc_atomic(sizeof(T))
    struct FileHandle: public NativeInstance {
        uv_fs_t* fd;
    };

    struct ReadData {
        uv_buf_t buffer;
        GcPtr<Future> future;
    };

    struct ReadFileData {
        uv_buf_t buffer;
        Future* future;
    };


    GcPtr<NativeStruct> FILE_HANDLE_STRUCT;


    auto translate_open_flags(const char* mode) {
        if (strcmp(mode, "r") == 0) {
            return UV_FS_O_RDONLY;
        }
        else if (strcmp(mode, "r+") == 0) {
            return UV_FS_O_RDWR;
        }
        else if (strcmp(mode, "w") == 0) {
            return UV_FS_O_WRONLY | UV_FS_O_CREAT | UV_FS_O_TRUNC;
        }
        else if (strcmp(mode, "w+") == 0) {
            return UV_FS_O_RDWR | UV_FS_O_CREAT | UV_FS_O_TRUNC;
        }
        else if (strcmp(mode, "a") == 0) {
            return UV_FS_O_APPEND | UV_FS_O_CREAT;
        }
        else if (strcmp(mode, "a+") == 0) {
            return UV_FS_O_APPEND | UV_FS_O_CREAT;
        }
        else {
            return UV_FS_O_RDONLY;
        }
    }


    const char *fs_open_doc = R"(
        open(path: String, mode: String) -> Future<FileHandle>
        Open a file.
        path: The path of the file.
        mode: The mode of the file.

        modes:
            r: Open file for reading.
            r+: Open file for reading and writing.
            w: Open file for writing. The file is created (if it does not exist) or truncated (if it exists).
            w+: Open file for reading and writing. The file is created (if it does not exist) or truncated (if it exists).
            a: Open file for appending. The file is created if it does not exist.
            a+: Open file for reading and appending. The file is created if it does not exist.
        Returns: A future that resolves to a FileHandle.
    )
    )";

    obj_result fs_open(const t_vector& args) {
        String* path;
        String* mode;
        TRY(parse_args(args, path, mode));

        auto open_req = (uv_fs_t* )GC_malloc_atomic(sizeof(uv_fs_t));

        auto future = Runtime::ins()->make_future();
        open_req->data = (void* )future.get();

        auto cb = [](uv_fs_t* req) {
            auto future = (Future *) req->data;

            if (req->result < 0) {
                future->set_value(make_error_t(fmt::format("Failed to open file '{}', {}", req->path, uv_strerror(req->result))));
                return;
            }
            else {
                auto f_h = FILE_HANDLE_STRUCT->create_instance<FileHandle>();
                f_h->fd = req;
                future->set_value(make_ok(f_h));
            }

//            uv_fs_req_cleanup(req);
//            GC_free(req);
            fmt::print("opened file\n");
        };

        fmt::print("opening file\n");
        auto flags = translate_open_flags(mode->get_value_ref().c_str());
        uv_fs_open(uv_default_loop(), open_req, path->get_value_ref().c_str(), flags, 0, cb);
        return future;
    }

    const char *fs_read_doc = R"(
        read(path: String, encoding: String)
        Read a file.
        path: The path of the file.
        encoding: The encoding of the file.
    )";

    obj_result fs_read(const t_vector& args) {
        FileHandle* f_h;
        TRY(parse_args(args, f_h));

        auto read_req = ALLOC_ATOMIC(uv_fs_t);
        auto future = Runtime::ins()->make_future();

        //blocking. do we need to call uv_fs_stat
        auto file_size = std::filesystem::file_size(f_h->fd->path);
        auto buffer = uv_buf_init((char*)GC_malloc_atomic(file_size), file_size);

        auto r_data = ALLOC_ATOMIC(ReadData);
        r_data->buffer = buffer;
        r_data->future = future;

        read_req->data = (void* )r_data;


        auto cb = [](uv_fs_t* req) {
            auto data = (ReadData*)req->data;

            if (req->result == -1) {
                data->future->set_value(make_error_t(uv_strerror(req->result)));
            }
            else{
                data->future->set_value(make_ok<String>(data->buffer.base, data->buffer.len));
            }
            GC_free(data->buffer.base);
            GC_free(req);
            GC_free(data);
            uv_fs_req_cleanup(req);
        };

        uv_fs_read(uv_default_loop(), read_req, f_h->fd->result, &buffer, 1, -1, cb);
        return future;
    }

    const char *fs_close_doc = R"(
        close(file: FileHandle)
        close a file.
    )";

    obj_result fs_close(const t_vector& args) {
        FileHandle* f_h;
        TRY(parse_args(args, f_h));

        auto close_req = ALLOC_ATOMIC(uv_fs_t);
        auto future = Runtime::ins()->make_future();

        close_req->data = (void* )future.get();

        auto cb = [](uv_fs_t* req) {
            auto future = (Future *) req->data;

            if (req->result < 0) {
                future->set_value(make_error_t(fmt::format("Failed to close file '{}', {}", req->path, uv_strerror(req->result))));
                return;
            }
            else {
                future->set_value(Runtime::ins()->C_NONE_RESULT);
            }

            uv_fs_req_cleanup(req);
            GC_free(req);
        };

        uv_fs_close(uv_default_loop(), close_req, f_h->fd->result, cb);
        return future;
    }


    auto cb_read(uv_fs_t* req){
        // part 3 close
        auto data = (ReadFileData*)req->data;

        if (req->result == -1) {
            data->future->set_value(make_error_t(uv_strerror(req->result)));
        }
        else{
            data->future->set_value(make_ok<String>(data->buffer.base, data->buffer.len));
        }
        GC_free(data->buffer.base);
        GC_free(req);
        GC_free(data);
        uv_fs_req_cleanup(req);
    }

    auto cb_open(uv_fs_t* req){
        // part 2 read

        auto data = (ReadFileData*)req->data;

        if (req->result < 0) {
            data->future->set_value(make_error_t(fmt::format("Failed to open file '{}', {}", req->path, uv_strerror(req->result))));
            return;
        }

        auto file_size = std::filesystem::file_size(req->path);
        data->buffer = uv_buf_init((char*)GC_malloc_atomic(file_size), file_size);

        auto read_req = ALLOC_ATOMIC(uv_fs_t);
        read_req->data = (void* )data;


        uv_fs_read(uv_default_loop(), read_req, req->result, &data->buffer, 1, -1, cb_read);
//        uv_fs_req_cleanup(req);
//        GC_free(req);
    }

    const char *read_file_doc = R"(
        read_file(path: String) -> String
        Read a file.
        path: The path of the file.
    )";

    auto fs_read_file(const t_vector& args) -> obj_result {
        String* path;
        TRY(parse_args(args, path));

        auto future = Runtime::ins()->make_future();

        // part 1 - open file
        auto request_data = (ReadFileData*) GC_malloc(sizeof(ReadFileData));
        auto the_request = ALLOC_ATOMIC(uv_fs_t);
        the_request->data = (void* )request_data;
        request_data->future = future.get();

        uv_fs_open(uv_default_loop(), the_request, path->get_value_ref().c_str(), UV_FS_O_RDONLY, 0, cb_open);
        return future;
    }

}


EXPORT void bond_module_init(bond::Context *ctx, bond::Vm* current_vm, bond::Mod &mod) {
    GC_INIT();
    //GC_disable();
    bond::Runtime::ins()->set_runtime(current_vm->runtime());
    bond::set_current_vm(current_vm);

    bond::io::FILE_HANDLE_STRUCT = bond::make_immortal<bond::NativeStruct>("FileHandle", "FileHandle()", bond::c_Default<bond::io::FileHandle>);


    auto fs_mod = bond::Mod("fs");
    fs_mod.function("open", bond::io::fs_open, bond::io::fs_open_doc);
    fs_mod.function("read", bond::io::fs_read, bond::io::fs_read_doc);
    fs_mod.function("close", bond::io::fs_close, bond::io::fs_close_doc);
    fs_mod.function("read_file", bond::io::fs_read_file, bond::io::read_file_doc);


    mod.add("fs", fs_mod.build()->as<bond::Object>());

//    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    current_vm->set_start_event_loop_cb([]() {
        uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    });

    current_vm->runtime()->add_exit_callback([]() {
        uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    });
}