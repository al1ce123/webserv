#!/usr/bin/php
<?php

// Define your username and password
$valid_username = 'admin';
$valid_password = 'admin';

// Output the HTML content
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Result</title>
</head>
<body>
HTML;

// Check if username and password are provided
if (isset($_POST['username']) && isset($_POST['password'])) {
    $username = $_POST['username'];
    $password = $_POST['password'];
    
    // Validate username and password
    if ($username === $valid_username && $password === $valid_password) {
        // Authentication successful
        echo "<p>Authentication successful. Welcome, $username!</p>";
        // You can redirect to another page here if needed
    } else {
        // Authentication failed
        echo "<p>Invalid username or password. Please try again.</p>";
    }
} else {
    // Username or password not provided
    echo "<p>Please provide both username and password.</p>";
}

// Close the HTML content
echo <<<HTML
</body>
</html>
HTML;

?>

