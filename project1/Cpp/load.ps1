# Load environment variables
. ..\setenv.ps1

# Change directory to build
Set-Location -Path .\build

# Remove LoadData if it exists
if (Test-Path .\LoadData) { Remove-Item .\LoadData }

# Run make command
# make

# Get the current directory path
$CSV_PATH = (Get-Location).Path

# Execute the commands in a timed block
Measure-Command {
    # Drop tables if they exist
    psql --command="DROP TABLE IF EXISTS cards;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="DROP TABLE IF EXISTS passengers;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="DROP TABLE IF EXISTS rides;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="DROP TABLE IF EXISTS lines;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="DROP TABLE IF EXISTS line_details;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="DROP TABLE IF EXISTS stations;" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME

    # Run LoadData
    .\LoadData

    # Copy data from CSV files to tables
    psql --command="\COPY cards FROM '$CSV_PATH\cards.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="\COPY passengers FROM '$CSV_PATH\passengers.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="\COPY rides FROM '$CSV_PATH\rides.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="\COPY lines FROM '$CSV_PATH\lines.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="\COPY line_details FROM '$CSV_PATH\line_details.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
    psql --command="\COPY stations FROM '$CSV_PATH\stations.csv' WITH (FORMAT CSV, HEADER, DELIMITER ',');" postgresql://$env:DB_USER:$env:DB_PASSWORD@$env:DB_HOST:$env:DB_PORT/$env:DB_NAME
} | Format-List