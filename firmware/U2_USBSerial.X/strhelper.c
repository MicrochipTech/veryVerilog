#include <xc.h>
#include <stdbool.h>
#include <string.h>

#if CITOA_NEEDED
// A utility function to reverse a string

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

// Implementation of citoa()

char* citoa(int num, char* str, int num_base) {
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with num_base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && num_base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % num_base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / num_base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}
#endif

char* u8toazpad(uint8_t num, uint8_t minimumNumDigits, char* buf) {
    if (buf) {
        if (minimumNumDigits > 3) {
            minimumNumDigits = 3;
        }
        if (num > 99) {
            *buf++ = num / 100 + '0';
            num -= (num / 100) * 100;
            minimumNumDigits = 3;
        } else {
            if (minimumNumDigits >= 3) {
                *buf++ = '0';
            }
        }
        if (num > 9) {
            *buf++ = num / 10 + '0';
            num -= (num / 10) * 10;
            minimumNumDigits = 2;
        } else {
            if (minimumNumDigits >= 2) {
                *buf++ = '0';
            }
        }
        if (num) {
            *buf++ = num + '0';
        } else {
            if (minimumNumDigits) {
                *buf++ = '0';
            }
        }
        *buf = (char) 0;
    }
    return buf;
}
