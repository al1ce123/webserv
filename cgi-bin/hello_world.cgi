#!/usr/bin/env python3.12

# import cgi
# import cgitb

# Enable detailed error messages to be displayed in the browser
# cgitb.enable()

# # Print HTTP header
# print("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!")

# # Parse request parameters
# form = cgi.FieldStorage()

# # Print request parameters
# print("Request Parameters:")
# for key in form.keys():
#     print(f"{key}: {form[key].value}")



# Print the HTTP response
print("HTTP/1.1 200 OK")
print("Content-Type: text/plain")
print("Content-Length: 12")
print()
print("Hello world!")