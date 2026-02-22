#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HANDLE hIn, hOut, hErr;

static void wstr(HANDLE h, const char *s) {
    DWORD n = 0, w;
    while (s[n]) n++;
    WriteFile(h, s, n, &w, 0);
}

static int readLine(char *buf, int max) {
    DWORD r; char c; int n = 0;
    while (n < max - 1) {
        if (!ReadFile(hIn, &c, 1, &r, 0) || r == 0) return -1;
        if (c == '\r') continue;
        if (c == '\n') break;
        buf[n++] = c;
    }
    buf[n] = 0;
    return n;
}

static const char *skipws(const char *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

static const char *parseNum(const char *p, double *out) {
    double r = 0, frac = 1;
    int neg = 0;
    p = skipws(p);
    if (*p == '-') { neg = 1; p++; }
    else if (*p == '+') p++;
    while (*p >= '0' && *p <= '9') { r = r * 10 + (*p++ - '0'); }
    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') { frac /= 10; r += (*p++ - '0') * frac; }
    }
    *out = neg ? -r : r;
    return p;
}

static void numToStr(double v, char *buf) {
    char *p = buf;
    char tmp[32];
    int n;

    if (v < 0) { *p++ = '-'; v = -v; }

    long long ip = (long long)v;
    double   fp  = v - (double)ip;

    n = 0;
    long long x = ip;
    if (x == 0) tmp[n++] = '0';
    else while (x > 0) { tmp[n++] = '0' + (int)(x % 10); x /= 10; }
    for (int i = n - 1; i >= 0; i--) *p++ = tmp[i];
	
    if (fp > 1e-9) {
        *p++ = '.';
        for (int d = 0; d < 8; d++) {
            fp *= 10;
            int dig = (int)fp;
            *p++ = '0' + dig;
            fp -= dig;
        }
        while (*(p-1) == '0') p--;
        if (*(p-1) == '.') p--;
    }

    *p++ = '\n'; *p = 0;
}

void entry(void) {
    hIn  = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hErr = GetStdHandle(STD_ERROR_HANDLE);

    wstr(hOut, "Calc: <number> <+,-,*,/> <number> q = quit\n");

    char line[128], result[64];

    for (;;) {
        wstr(hOut, "> ");
        if (readLine(line, sizeof(line)) < 0) break;
        if (!line[0]) continue;
        if (line[0] == 'q' || line[0] == 'Q') break;

        double a, b;
        const char *p = parseNum(line, &a);
        p = skipws(p);
        if (!*p) { wstr(hErr, "Error\n"); continue; }
        char op = *p++;
        parseNum(p, &b);

        double res;
        switch (op) {
            case '+': res = a + b; break;
            case '-': res = a - b; break;
            case '*': res = a * b; break;
            case '/':
                if (b == 0.0) { wstr(hErr, "Error\n"); continue; }
                res = a / b; break;
            default:
                wstr(hErr, "Error\n"); continue;
        }

        wstr(hOut, "= ");
        numToStr(res, result);
        wstr(hOut, result);
    }

    ExitProcess(0);
}