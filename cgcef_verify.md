% CGCEF_VERIFY(1) Cyber Grand Challenge Manuals
% April 17, 2014

# NAME

cgcef_verify file

# DESCRIPTION

cgcef_verify is a utility to verify that a file is a legal Cyber Grand Challenge Executable Format file; i.e. a Challenge Binary.
cgcef_verify will highlight incompatible portions of a CGCEF file and warn about sections which may be deprecated in the future or are optional.

## ARGUMENTS

file
: The file to verify

## EXIT STATUS

0
: The file is legal CGCEF

1
: The file is not legal CGCEF

# SEE ALSO

`cgcef` (1)

For more information relating to DARPA's Cyber Grand Challenge, please visit <http://www.darpa.mil/cybergrandchallenge/>

