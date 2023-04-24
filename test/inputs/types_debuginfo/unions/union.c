union intParts {
  int theInt;
  char bytes[sizeof(int)];
};

union intParts parts;
