#!/bin/bash

# Output the HTTP headers
echo "Content-Type: text/html"
echo ""

# Output the HTML content
cat << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Hello CGI</title>
</head>
<body>
    <h1>Hello, CGI!</h1>
    <p>The current date and time is: $(date)</p>
    <p>Your query string is: $QUERY_STRING</p>
</body>
</html>
EOF
