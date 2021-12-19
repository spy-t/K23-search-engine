# Παραδοτέο 2

## Αρχεία

- `/src/core.cpp`

## Build SIGMOD test driver

- Initialize
```bash
meson build -Dbuildtype=release
```

- Build (το core library χτίζεται σαν static library)
```bash
cd build && ninja core_test
```

- Εκτέλεση με το αρχείο small_test.txt του SIGMOD
```bash
./core_test ../src/test/resources/small_test.txt
```
Τα αποτελέσματα θα βρίσκονται στο `/build/result.txt`

- Leak checks (AddressSanitizer)
```bash
meson configure -Db_sanitize=address;
ninja core_test
```
Εκτέλεση με τον ίδιο τρόπο (δεν έχει διορθωθεί το leak του test driver του SIGMOD)

## Build core library σαν shared library για χρήση με άλλη main

- Build (έχοντας ακολουθήσει το βήμα Initialize από το προηγούμενο section)
```bash
cd build && ninja libcore.so
```
Το shared object file θα βρίσκεται στο `/build/libcore.so`
