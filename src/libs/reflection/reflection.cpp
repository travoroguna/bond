#include <filesystem>
#include "../../bond.h"


using namespace bond;
GarbageCollector *m_gc;

class IntegerType : public Object {

};

class TypeOf : public Object {
public:
    std::string str() override {
        return "TypeOf";
    }

    void mark() override {
        Object::mark();
    }

    void unmark() override {
        Object::unmark();
    }

//    GcPtr<Object> type_of(const GcPtr<Object> &obj) {
//        if (obj->is<Integer>()) {
//            return m_gc->make<IntegerType>();
//        }
//
//        else if(obj->is<String>()) {
//            return m_gc->make<StringType>();
//        }
//
//        else if (obj->is<Float>()) {
//            return m_gc->make<FloatType>();
//        }
//
//
//    }
};


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    GarbageCollector::instance().set_gc(ctx->gc());

    std::unordered_map<std::string, GcPtr<Object>> io_module = {

    };

    auto module = ctx->gc()->make<Module>(path, io_module);
    ctx->add_module(path, module);
}