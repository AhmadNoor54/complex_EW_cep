#!/bin/bash

# Set email parameters
recipient="ahmad4309990@cloud.neduet.edu.pk"
subject="Test Email"
message="This is a test email sent from a Bash script."

# Use the mail command to send the email
echo "$message" | mail -s "$subject" "$recipient"

# Check the exit status
if [ $? -eq 0 ]; then
    echo "Email sent successfully."
else
    echo "Failed to send email."
fi