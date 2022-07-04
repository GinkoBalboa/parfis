#ifndef PARFIS_GTEST_DEFINE_H
#define PARFIS_GTEST_DEFINE_H

// On some systems colors are not shown, so turn this off
#define USE_COLORS 0
#define GTEST_BOX "[       >>>] "


#if USE_COLORS == 1
#define ANSI_TXT_GRN "\033[0;32m" // Green
#define ANSI_TXT_YLW "\033[0;33m" // Yellow
#define ANSI_TXT_RED "\033[0;31m" // Red
#define ANSI_TXT_MGT "\033[0;35m" //Magenta
#define ANSI_TXT_DFT "\033[0;0m" //Console default
#define COUT_GTEST_GRN ANSI_TXT_GRN << GTEST_BOX << ANSI_TXT_DFT
#define COUT_GTEST_YLW ANSI_TXT_YLW << GTEST_BOX << ANSI_TXT_DFT
#define COUT_GTEST_RED ANSI_TXT_RED << GTEST_BOX << ANSI_TXT_DFT
#else
#define COUT_GTEST_GRN GTEST_BOX
#define COUT_GTEST_YLW GTEST_BOX
#define COUT_GTEST_RED GTEST_BOX
#endif

#endif // PARFIS_GTEST_DEFINE_H