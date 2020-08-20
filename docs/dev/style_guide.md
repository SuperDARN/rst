<!---
(C) copyright 2020 SuperDARN Canada, University of Saskatchewan
author: Marina Schmidt
(C) copyright 2020 UNIS
author: Emma Bland
-->

# Style Guide 

## Commenting 

When making comments in C please use the two different styles: 

1. Inline comments: these comments are used for make small comments about certain lines/blocks of code 
    ```C
    // creating an empty float array to hold the data
    float data = []
    ```
2. Documenting comments: these comments are for documenting purposes and found above the function name known as the *function header*. The header will include: 
  - Description: in lamen's terms what does this function do
  - Parameters: list out all parameters name, type and what they are suppose to be and what value you are expecting if applicable 
  - post: description on anything going on in the function that may be manipulating the data 
  - returns: description on what is returned from this function 
  - errors: if a error occurs list the potential error code and what it means 
 
  ```C
  /**
  * This function allocates memory for the data array
  *
  * @param array_ptr: array pointer for which the memory will be allocated to
  * @param array_length: the length of the array 
  *
  * @post: this function will allocate an address to a chunk of memory on the heap to array_ptr 
  * 
  * @return: nothing this is a void function
  * @error: -1 when memory cannot be allocated, stored in errno 
  **/
  ```

## Naming Convensions 

**directory names**:

**filenames**:

**function names**:

**variable names**: should be readable and understandable. Avoid single letter variable names unless there is a formula or comment to indicate why the single letter was chosen. 

### Common C variable abbreviations

## Spacing and Indenting 

C is a type strict language meaning variable types need to be defined and the syntax is strict. Unlike python, you have to use `{}` braces and indicate the variable type `int`. However, indentation is not strict and inline conditional (`if`, `for`, `while`) statements are allowed. Because of this C code can be very messy and hard to read even if you are use to programming in C. Thus we encourage the following rules: 

1. Indent the code especially if nested. This helps with readability especially if the code is long. 
Example: 
Do this
``` C
if (array != NULL)
{
    for(int i=0, i<array_len, i++)
    {
        do something… 
    }
}
```

Do not do this
``` C
if (array != NULL){
for(int i=0, i<array_len, i++){
do something… 
}
}
```


!!! Note
    Spaces and tabs can be different indentations and can cause a mess of the code to different people. To stay consistent it is encouraged to change the tab key to equal four spaces. This will help with any python coding the future. 

2. Avoid one line conditional statements. These can be missed when reviewing the code or looking for a bug. 
Make things as obvious as possible. 
Example:
do this
  ``` C
  if (counter < 10 )
  {
     // Add one to the current counter value
     counter += 1; 
  }
  ```
Don't do this
  ``` C
  if (counter < 10) counter += 1;
  ```

3. Add spaces after commas. This will help with readability. 

## Organizing the Code 

