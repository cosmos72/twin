/*
 * Helper function for X11_InitHW that verifies that hwName begins with the expected "-hw=..."
 * prefix string and returns the number of characters of said prefix.  Returns -1 if hwName does not
 * begin with the expected prefix.
 */
static int check_HW_Name(char *hwName) {
    if (strncmp(hwName, "-hw=X11", 7) == 0) {
        return 7;
    }
    if (strncmp(hwName, "-hw=X", 5) == 0) {
        return 5;
    }
    return -1;
}
