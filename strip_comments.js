const fs = require('fs');

// 1. Get command line arguments
const args = process.argv.slice(2);

if (args.length !== 2) {
    console.error('Usage: node strip_comments_clean.js <input_file> <output_file>');
    process.exit(1);
}

const inputFile = args[0];
const outputFile = args[1];

// Regex for comments (same as before)
const COMMENT_REGEX = /("(?:\\[\s\S]|[^"\\])*"|'(?:\\[\s\S]|[^'\\])*')|(\/\*[\s\S]*?\*\/)|(\/\/.*)/g;

// Regex for empty lines:
// ^        = Start of line
// [ \t]* = Zero or more spaces or tabs (horizontal whitespace only)
// [\r\n]+  = One or more newline characters
const EMPTY_LINE_REGEX = /^[ \t]*[\r\n]+/gm;

fs.readFile(inputFile, 'utf8', (err, data) => {
    if (err) {
        console.error(`Error reading file: ${err.message}`);
        process.exit(1);
    }

    // Step 1: Strip comments
    // We replace multi-line comments with a single space to be safe, 
    // and single-line comments with nothing.
    let strippedData = data.replace(COMMENT_REGEX, (match, strLiteral, multiLine, singleLine) => {
        if (strLiteral) return strLiteral; // Preserve strings
        if (multiLine) return ' ';         // Replace block comments with a space
        if (singleLine) return '';         // Remove single line comments entirely
        return match;
    });

    // Step 2: Remove empty or whitespace-only lines
    // We do this AFTER removing comments, because removing a comment 
    // often leaves an empty line behind.
    strippedData = strippedData.replace(EMPTY_LINE_REGEX, '');

    // Step 3: Write the result
    fs.writeFile(outputFile, strippedData, (err) => {
        if (err) {
            console.error(`Error writing file: ${err.message}`);
            process.exit(1);
        }
        console.log(`Success! Processed ${inputFile} -> ${outputFile}`);
    });
});