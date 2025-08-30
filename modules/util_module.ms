function power(int base, int exp) { 
    result = 1; 
    i = 0; 
    while (i < exp) { 
        result = result * base; 
        i = i + 1; 
    } 
    return result; 
} 
