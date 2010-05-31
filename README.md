# Function Renaming in PHP

A utility function for renaming functions for the purpose of stubbing out things that are untested

## Installation

* phpize
* ./configure --enable-rename
* make && make install

## Usage
    <?php
    function old_function() { }
    rename_function('old_function', 'new_function');