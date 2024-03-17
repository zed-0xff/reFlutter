auto &funcs = Array::Handle(cls.functions());
if (funcs.Length() > 1000) {
  continue;
}

char classText[2500000] = "";
String &supname = String::Handle();
name = cls.Name();
strcat(classText, cls.ToCString());

Class &supcls = Class::Handle();
supcls = cls.SuperClass();
if (!supcls.IsNull()) {
  supname = supcls.Name();
  strcat(classText, " extends ");
  strcat(classText, supname.ToCString());
}

const auto &interfaces = Array::Handle(cls.interfaces());
auto &interface = Instance::Handle();
for (intptr_t in = 0; in < interfaces.Length(); in++) {
  interface ^= interfaces.At(in);
  if (in == 0) {
    strcat(classText, " implements ");
  }
  if (in > 0) {
    strcat(classText, " , ");
  }
  strcat(classText, interface.ToCString());
}
strcat(classText, " {\n");

const auto &fields = Array::Handle(cls.fields());
auto &field = Field::Handle();
auto &fieldType = AbstractType::Handle();
String &fieldTypeName = String::Handle();
String &finame = String::Handle();
Instance &instance2 = Instance::Handle();
for (intptr_t f = 0; f < fields.Length(); f++) {
  field ^= fields.At(f);
  finame = field.name();
  fieldType = field.type();
  fieldTypeName = fieldType.Name();
  strcat(classText, "  ");
  strcat(classText, fieldTypeName.ToCString());
  strcat(classText, " ");
  strcat(classText, finame.ToCString());
  if (field.is_static()) {
    instance2 ^= field.StaticValue();
    strcat(classText, " = ");
    strcat(classText, instance2.ToCString());
    strcat(classText, " ;\n");
  } else {
    strcat(classText, " = ");
    strcat(classText, "nonstatic;\n");
  }
}

for (intptr_t c = 0; c < funcs.Length(); c++) {
  auto &func = Function::Handle();
  func = cls.FunctionFromIndex(c);
  String &signature = String::Handle();
  signature = func.InternalSignature();
  auto &codee = Code::Handle(func.CurrentCode());
  if (!func.IsLocalFunction()) {
    strcat(classText, " \n  ");
    strcat(classText, func.ToCString());
    strcat(classText, " ");
    strcat(classText, signature.ToCString());
    strcat(classText, " { \n\n              ");
    char append[70];
    snprintf(append, sizeof(append),
             " Code Offset: _kDartIsolateSnapshotInstructions + 0x%016" PRIxPTR
             "\n",
             static_cast<uintptr_t>(codee.MonomorphicUncheckedEntryPoint()));
    strcat(classText, append);
    strcat(classText, "       \n       }\n");
  } else {
    auto &parf = Function::Handle();
    parf = func.parent_function();
    String &signParent = String::Handle();
    signParent = parf.InternalSignature();
    strcat(classText, " \n  ");
    strcat(classText, parf.ToCString());
    strcat(classText, " ");
    strcat(classText, signParent.ToCString());
    strcat(classText, " { \n\n          ");
    char append[80];
    snprintf(append, sizeof(append),
             " Code Offset: _kDartIsolateSnapshotInstructions + 0x%016" PRIxPTR
             "\n",
             static_cast<uintptr_t>(codee.MonomorphicUncheckedEntryPoint()));
    strcat(classText, append);
    strcat(classText, "       \n       }\n");
  }
}
strcat(classText, " \n      }\n\n");

const Library &libr = Library::Handle(cls.library());
if (!libr.IsNull()) {
  auto &owner_class = Class::Handle();
  owner_class = libr.toplevel_class();
  auto &funcsTopLevel = Array::Handle(owner_class.functions());
  char pushTmp[1000];
  String &owner_name = String::Handle();
  owner_name = libr.url();
  snprintf(pushTmp, sizeof(pushTmp), "\'%s\',", owner_name.ToCString());
  if (funcsTopLevel.Length() > 0 && strstr(pushArr, pushTmp) == NULL) {
    strcat(pushArr, pushTmp);
    strcat(classText, "Library:");
    strcat(classText, pushTmp);
    strcat(classText, " {\n");
    for (intptr_t c = 0; c < funcsTopLevel.Length(); c++) {
      auto &func = Function::Handle();
      func = owner_class.FunctionFromIndex(c);
      String &signature = String::Handle();
      signature = func.InternalSignature();
      auto &codee = Code::Handle(func.CurrentCode());
      if (!func.IsLocalFunction()) {
        strcat(classText, " \n  ");
        strcat(classText, func.ToCString());
        strcat(classText, " ");
        strcat(classText, signature.ToCString());
        strcat(classText, " { \n\n              ");
        char append[70];
        snprintf(
            append, sizeof(append),
            " Code Offset: _kDartIsolateSnapshotInstructions + 0x%016" PRIxPTR
            "\n",
            static_cast<uintptr_t>(codee.MonomorphicUncheckedEntryPoint()));
        strcat(classText, append);
        strcat(classText, "       \n       }\n");
      } else {
        auto &parf = Function::Handle();
        parf = func.parent_function();
        String &signParent = String::Handle();
        signParent = parf.InternalSignature();
        strcat(classText, " \n  ");
        strcat(classText, parf.ToCString());
        strcat(classText, " ");
        strcat(classText, signParent.ToCString());
        strcat(classText, " { \n\n          ");
        char append[80];
        snprintf(
            append, sizeof(append),
            " Code Offset: _kDartIsolateSnapshotInstructions + 0x%016" PRIxPTR
            "\n",
            static_cast<uintptr_t>(codee.MonomorphicUncheckedEntryPoint()));
        strcat(classText, append);
        strcat(classText, "       \n       }\n");
      }
    }
    strcat(classText, " \n      }\n\n");
  }
}

struct stat entry_info;
int exists = 0;
if (stat("/data/data/", &entry_info) == 0 && S_ISDIR(entry_info.st_mode)) {
  exists = 1;
}

if (exists == 1) {
  pid_t pid = getpid();
  char path[64] = {0};
  snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
  FILE *cmdline = fopen(path, "r");
  if (cmdline) {
    char chm[264] = {0};
    char pat[264] = {0};
    char application_id[64] = {0};
    fread(application_id, sizeof(application_id), 1, cmdline);
    snprintf(pat, sizeof(pat), "/data/data/%s/dump.dart", application_id);
    do {
      FILE *f = fopen(pat, "a+");
      fprintf(f, "%s", classText);
      fflush(f);
      fclose(f);
      snprintf(chm, sizeof(chm), "/data/data/%s", application_id);
      chmod(chm, S_IRWXU | S_IRWXG | S_IRWXO);
      chmod(pat, S_IRWXU | S_IRWXG | S_IRWXO);
    } while (0);
    fclose(cmdline);
  }
}

if (exists == 0) {
  char pat[264] = {0};
  snprintf(pat, sizeof(pat), "%s/Documents/dump.dart", getenv("HOME"));
  OS::PrintErr("reFlutter dump file: %s", pat);
  do {
    FILE *f = fopen(pat, "a+");
    fprintf(f, "%s", classText);
    fflush(f);
    fclose(f);
  } while (0);
}
