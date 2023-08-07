import "core:bnk" as nk;


struct Window{
    var name;
    var rect;
    var flags;
    var children;

    fn init(name, rec, flag, children) {
        println(name, rec, flag, children);
        return Window(name, rec, flag, children);
    }

    fn begin(self) {
        if nk.begin(self.name, self.rect, self.flags) {
            for child in self.children {
                child.begin();
                child.end();
            }
        }
    }

    fn end(self) {
        nk.end();
    }
}

struct Button {
    var label;
    var callback;

    fn init(label, callback) {
        return Button(label, callback);
    }

    fn begin(self) {
        if nk.button_label(self.label) {
            self.callback();
        }
    }

    fn end(self) {}
}

struct LayoutRow {
    var height;
    var children;


    fn init(height, children) {
        return LayoutRow(height, children);
    }

    fn begin(self) {
        nk.layout_row_dynamic(self.height, self.children.size());
        for child in self.children {
            child.begin();
            child.end();
        }
    }

    fn end(self) {}
}


struct Label {
    var text;
    var alignment;

    fn init(text, alignment) {
        return Label(text, alignment);
    }

    fn begin(self) {
        nk.label(self.text, self.alignment);
    }

    fn end(self) {}
}

try nk.init(800, 600, "nk test");


var flags = nk.flags.WINDOW_BORDER | nk.flags.WINDOW_SCALABLE | nk.flags.WINDOW_MINIMIZABLE | nk.flags.WINDOW_TITLE | nk.flags.WINDOW_MOVABLE;
var rect = nk.rect(0.0, 0.0, 200.0, 200.0);

var window = Window.init("test", rect, flags,
[
    LayoutRow.init(30.0, [
        Button.init("button", fn() {
            println("button pressed");
        }),
        Button.init("button", fn() {
            println("button pressed");
        }),
        Button.init("button", fn() {
            println("button pressed");
        })
    ])

]);

while !nk.should_close() {
    nk.new_frame();
    window.begin();
    window.end();
    nk.render();
}

nk.de_init();