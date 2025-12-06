#ifndef UTILS_HPP
#define UTILS_HPP

#ifdef _WIN32
  #define CLS() system("cls");
#else
  #define CLS() system("clear");
#endif

#endif // UTILS_HPP