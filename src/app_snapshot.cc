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

char *buffer = nullptr;
intptr_t buffer_length = 0;
js.Steal(&buffer, &buffer_length);
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
      fprintf(f, "%s", buffer);
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
    fprintf(f, "%s", buffer);
    fflush(f);
    fclose(f);
  } while (0);
}
