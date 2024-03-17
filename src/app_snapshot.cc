ASSERT(code->IsCode());

auto &rClass = Class::Handle(func.Owner());
auto &rLib = Library::Handle(rClass.library());
auto &rlibName = String::Handle(rLib.url());
char offsetString[70];
snprintf(
    offsetString, sizeof(offsetString), "0x%016" PRIxPTR,
    static_cast<uintptr_t>(code->untag()->monomorphic_unchecked_entry_point_));

JSONWriter js;
js.OpenObject();
js.PrintProperty("method_name", func.UserVisibleNameCString());
js.PrintProperty("offset", offsetString);
js.PrintProperty("library_url", rlibName.ToCString());
js.PrintProperty("class_name", rClass.UserVisibleNameCString());
js.CloseObject();

char *classText = nullptr;
intptr_t buffer_length = 0;
js.Steal(&classText, &buffer_length);
