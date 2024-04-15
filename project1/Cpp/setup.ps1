# Create a new directory named lib
New-Item -ItemType Directory -Force -Path lib

# Change directory to lib
Set-Location -Path .\lib

# Download the file from the URL
Invoke-WebRequest -Uri https://github.com/jtv/libpqxx/archive/refs/tags/7.9.0.tar.gz -OutFile 7.9.0.tar.gz

# Extract the tar.gz file
# PowerShell does not have a built-in command to extract tar.gz files, you might need to use a tool like 7zip

# Rename the directory
Rename-Item -Path .\libpqxx-7.9.0 -NewName libpqxx

# Change directory to the parent directory
Set-Location -Path ..

# Create a new directory named build
New-Item -ItemType Directory -Force -Path build

# Change directory to build
Set-Location -Path .\build

# Run cmake command
# cmake ..

# Run make command with 16 jobs
# make -j 16