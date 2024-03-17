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
