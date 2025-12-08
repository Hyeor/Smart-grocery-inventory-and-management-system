# Product Name Validation Enhancement

## Feature Added: Minimum 3-Word Product Names

### Overview
Product names must now contain **at least 3 words** to ensure consistency and clarity in inventory management.

### Examples of Valid Product Names
✓ "Pisang Abu Susu"  
✓ "Buah Delima Segar"  
✓ "Apel Merah Impor"  
✓ "Jeruk Mandarin Manis"  
✓ "Mangga Harum Manis"  

### Examples of Invalid Product Names
✗ "Pisang"  
✗ "Buah Delima"  
✗ "Apel"  
✗ "Orange"  

## Implementation Details

### Code Changes

**File Modified:** `source/Product.cpp`

**New Features:**
1. Added `#include <sstream>` for string stream processing
2. Added `countWords()` helper function to validate word count
3. Enhanced product name input with validation loop

### How It Works

```cpp
// Helper function counts words in product name
int countWords(const string& str) {
    stringstream ss(str);
    string word;
    int count = 0;
    while (ss >> word) {
        count++;
    }
    return count;
}

// Validation loop for product name
while (countWords(name) < 3) {
    cout << "Error: Product name must have at least 3 words!" << endl;
    cout << "Enter Product Name (at least 3 words): ";
    getline(cin, name);
}
```

## User Experience

### When Adding a Product

```
Enter Product Name (at least 3 words, e.g. 'Pisang Abu Susu'): Apel
Error: Product name must have at least 3 words!
Enter Product Name (at least 3 words): Apel Merah
Error: Product name must have at least 3 words!
Enter Product Name (at least 3 words): Apel Merah Impor
Enter Cost Price: 15000
Enter Sell Price: 20000
Enter Quantity: 50
Product Added Successfully.
```

## Validation Rules

| Input | Result |
|-------|--------|
| Single word (e.g., "Pisang") | ✗ Rejected |
| Two words (e.g., "Pisang Abu") | ✗ Rejected |
| Three+ words (e.g., "Pisang Abu Susu") | ✓ Accepted |
| Multiple spaces | Handled correctly |
| Empty input | ✗ Rejected (0 words) |

## Benefits

1. **Better Data Quality** - Consistent, descriptive product names
2. **Easier Identification** - More detailed product descriptions
3. **Professional Appearance** - Standardized naming convention
4. **Search Friendly** - More words = better search capability

## Recompilation Status

✅ **Application Recompiled Successfully**
- Executable: `build/main.exe`
- Size: 188.48 KB
- All input validations active

## How to Test

1. Run: `scripts\run.bat`
2. Choose "Add Product" from menu
3. Try entering:
   - One word (should fail)
   - Two words (should fail)
   - Three+ words (should succeed)

