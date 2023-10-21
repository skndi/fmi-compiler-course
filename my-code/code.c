void addArrs(int *foo, int *bar, int *out, int size) {
  for (int i = 0; i < size; ++i) {
    out[i] = foo[i] + bar[i];
  }
}