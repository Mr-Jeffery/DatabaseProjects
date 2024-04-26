
-- Ride table adjustments
-- Change the column types to match the ER diagram's references to the stations' station_id
ALTER TABLE stations DROP CONSTRAINT stations_pkey CASCADE;
ALTER TABLE stations ADD COLUMN station_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;

-- Step 1: Altering 'rides' table
-- Note: Ensure there is no primary key on 'rides' called 'rides_pkey' before uncommenting the below line
-- ALTER TABLE rides DROP CONSTRAINT IF EXISTS rides_pkey CASCADE;

-- Add a new primary key column if it doesn't exist (Assuming 'ride_id' is the desired primary key according to the ER diagram)
ALTER TABLE rides ADD COLUMN IF NOT EXISTS ride_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;

-- Change 'start_station' and 'end_station' to BIGINT and add foreign keys
-- This assumes 'start_station' and 'end_station' are currently storing valid station ids as VARCHAR.
-- If they contain station names, additional steps would be needed to map these to station ids.
-- Step 1: Add new columns for station IDs
ALTER TABLE rides
    ADD COLUMN start_station_id BIGINT,
    ADD COLUMN end_station_id BIGINT;

-- Step 2: Populate the new columns with station IDs based on station names
UPDATE rides
SET start_station_id = (SELECT station_id FROM stations WHERE stations.name = rides.start_station),
    end_station_id = (SELECT station_id FROM stations WHERE stations.name = rides.end_station);

-- Step 3: Drop the old VARCHAR columns that contained the station names
ALTER TABLE rides
    DROP COLUMN start_station,
    DROP COLUMN end_station;

-- Step 4: Add foreign key constraints to the new integer columns
ALTER TABLE rides
    ADD CONSTRAINT fk_rides_start_station FOREIGN KEY (start_station_id) REFERENCES stations(station_id),
    ADD CONSTRAINT fk_rides_end_station FOREIGN KEY (end_station_id) REFERENCES stations(station_id);

-- Optionally, if you want to set the new station_id columns as NOT NULL after ensuring all data is correct
ALTER TABLE rides
    ALTER COLUMN start_station_id SET NOT NULL,
    ALTER COLUMN end_station_id SET NOT NULL;


-- Alter 'lines' table
-- Add a new primary key column 'line_ID' if it doesn't exist
ALTER TABLE lines DROP CONSTRAINT lines_pkey CASCADE;
ALTER TABLE lines ADD COLUMN IF NOT EXISTS line_ID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY ;
-- Alter 'line_details' table
-- Add new columns for referencing 'lines' and 'stations' by ID
ALTER TABLE line_details
    ADD COLUMN line_id BIGINT,
    ADD COLUMN station_id BIGINT;

-- Populate the new columns with the correct IDs from 'lines' and 'stations'
UPDATE line_details
SET line_id = (SELECT line_ID FROM lines WHERE lines.name = line_details.line_name),
    station_id = (SELECT station_id FROM stations WHERE stations.name = line_details.station_name);

-- Drop the old VARCHAR columns that contained the names
ALTER TABLE line_details
    DROP COLUMN line_name,
    DROP COLUMN station_name;

-- Add foreign key constraints to the new columns
ALTER TABLE line_details
    ADD CONSTRAINT fk_line_details_line FOREIGN KEY (line_id) REFERENCES lines(line_ID),
    ADD CONSTRAINT fk_line_details_station FOREIGN KEY (station_id) REFERENCES stations(station_id);

-- Optionally, set the new columns as NOT NULL after verifying all data is correctly populated
ALTER TABLE line_details
    ALTER COLUMN line_id SET NOT NULL,
    ALTER COLUMN station_id SET NOT NULL;
-- Assuming the 'exits' table needs to reference 'stations' by 'station_id'
-- Alter 'exits' table
-- Add a foreign key constraint if it's not already there
-- Step 1: Add the 'station_id' column to 'exits' table
ALTER TABLE exits
    ADD COLUMN IF NOT EXISTS station_id BIGINT;

-- Step 2: Populate the 'station_id' column using the 'station_name' as a reference
UPDATE exits
SET station_id = (SELECT station_id FROM stations WHERE stations.name = exits.station_name);

-- Check if there are any rows that didn't get a matching station_id and address those before continuing
SELECT * FROM exits WHERE station_id IS NULL;

-- Step 3: Drop the existing primary key constraint if it exists
-- Note: You need to replace 'exits_pkey' with the actual constraint name if it's different
ALTER TABLE exits
    DROP CONSTRAINT IF EXISTS exits_pkey cascade ;

-- Step 4: Add the new composite primary key
ALTER TABLE exits
    ADD PRIMARY KEY (station_id, name);

-- Optional: Add foreign key constraint from 'station_id' to 'stations' table
-- Ensure the 'station_id' column has been populated with valid references before adding the constraint
ALTER TABLE exits
    ADD CONSTRAINT fk_exits_station_id FOREIGN KEY (station_id) REFERENCES stations(station_id);

-- Optional: Clean up 'station_name' column if it's no longer needed
ALTER TABLE exits
    DROP COLUMN IF EXISTS station_name;

-- Step 1: Add an 'bus_station_ID' column with the BIGINT type and set it as the primary key
ALTER TABLE bus_stations DROP CONSTRAINT bus_stations_pkey CASCADE;
ALTER TABLE bus_stations
    ADD COLUMN IF NOT EXISTS bus_station_ID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;


-- Assuming 'exits' already has a composite primary key on [station_id, name]
-- and 'bus_stations' has a primary key on bus_station_ID

-- Step 1: Modify 'exit_details' to include 'bus_station_ID' column if it doesn't already exist
ALTER TABLE exit_details
    ADD COLUMN IF NOT EXISTS bus_station_ID BIGINT;

-- Step 2: Populate 'bus_station_ID' in 'exit_details' based on a join with 'bus_stations' by name
-- This assumes that 'bus_station_name' is a column in 'exit_details' that matches the 'name' in 'bus_stations'
UPDATE exit_details ed
SET bus_station_ID = bs.bus_station_ID
FROM bus_stations bs
WHERE bs.name = ed.bus_station_name;

ALTER TABLE exit_details
    ADD COLUMN IF NOT EXISTS station_id BIGINT;

-- Step 2: Populate the 'station_id' column using the 'station_name' as a reference
UPDATE exit_details
SET station_id = (SELECT station_id FROM stations WHERE stations.name = exit_details.station_name);


-- Step 3: Add the composite primary key to 'exit_details' if it doesn't already exist
ALTER TABLE exit_details drop CONSTRAINT exit_details_pkey CASCADE;
Alter table exit_details ADD PRIMARY KEY (station_id, name, bus_station_ID);

-- Step 4: Establish the foreign key relationships for 'exit_details'
ALTER TABLE exit_details
    ADD CONSTRAINT fk_exit_details_station FOREIGN KEY (station_id,name) REFERENCES exits(station_id,name),
    ADD CONSTRAINT fk_exit_details_bus_station FOREIGN KEY (bus_station_ID) REFERENCES bus_stations(bus_station_ID);

ALTER TABLE exit_details
    DROP COLUMN bus_station_name,
    DROP COLUMN station_name;


-- Query to check entries that contain specific unwanted patterns
SELECT DISTINCT bus_line_name
FROM bus_line_details
WHERE bus_line_name LIKE '%出入口%'
   OR bus_line_name LIKE '%（新增）%'
   OR bus_line_name LIKE '%（原%'
   Or bus_line_name Like '%（右环）%'
   Or bus_line_name Like '%（左环）%';

-- Update to remove '(新增)' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（新增）.*$', '')
WHERE bus_line_name LIKE '%（新增）%';

-- Update to remove any pattern ending with '出入口' (including preceding numbers and letters)
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '[A-Z]*[0-9]*出入口$', '')
WHERE bus_line_name LIKE '%出入口%';

-- Update to remove '(原' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（原.*$', '')
WHERE bus_line_name LIKE '%（原%';

-- Update to remove '(右)' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（右环）.*$', '')
WHERE bus_line_name LIKE '%(右环)%';

-- Update to remove '(右)' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（右环）.*$', '')
WHERE bus_line_name LIKE '% (右环)%';

-- Update to remove '(左)' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（左环）.*$', '')
WHERE bus_line_name LIKE '%(左环)%';
-- Update to remove '(左)' and everything following it
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '（左环）.*$', '')
WHERE bus_line_name LIKE '% (左环)%';


-- Additional regex to clean up '(原' followed by any characters
UPDATE bus_line_details
SET bus_line_name = REGEXP_REPLACE(bus_line_name, '\(原.*$', '')
WHERE bus_line_name LIKE '%\(原%';



-- Step 5: Insert unique bus_line_name values into the bus_lines table
INSERT INTO bus_lines
SELECT DISTINCT bus_line_name FROM bus_line_details;

--how to use rename

ALTER TABLE bus_lines RENAME COLUMN name TO bus_line_name;

-- Step 1: Drop the existing primary key constraint
ALTER TABLE bus_lines DROP CONSTRAINT IF EXISTS bus_lines_pkey CASCADE;

-- Step 2: Add the bus_lines_ID column as the primary key
ALTER TABLE bus_lines ADD COLUMN IF NOT EXISTS bus_lines_ID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;


-- Now we need to update 'bus_line_details' with the new 'bus_lines_ID' based on the 'bus_line_name'.
-- First, we add the 'bus_line_id' column to 'bus_line_details'.
ALTER TABLE bus_line_details
    ADD COLUMN IF NOT EXISTS bus_line_id BIGINT;

-- Populate the new 'bus_line_id' column by matching the name from 'bus_lines'.
UPDATE bus_line_details bld
SET bus_line_id = bl.bus_lines_ID
FROM bus_lines bl
WHERE bl.bus_line_name = bld.bus_line_name;

-- Assuming 'bus_line_name' in 'bus_line_details' is not needed anymore and all references are updated,
-- we can safely drop the 'bus_line_name' column.

-- Add a foreign key constraint from 'bus_line_details' to 'bus_lines'.
ALTER TABLE bus_line_details
    ADD CONSTRAINT fk_bus_line_details_bus_line_id FOREIGN KEY (bus_line_id) REFERENCES bus_lines(bus_lines_ID);

ALTER TABLE bus_line_details
    ADD COLUMN IF NOT EXISTS bus_station_ID BIGINT;

-- Assuming 'bus_station_name' and 'bus_line_name' correspond to 'name' in 'bus_stations' and 'bus_lines' respectively
-- Populate the 'bus_station_ID' and 'bus_line_ID' columns
UPDATE bus_line_details bld
SET bus_station_ID = bs.bus_station_ID
FROM bus_stations bs
WHERE bld.bus_station_name = bs.name;
-- Add a foreign key constraint from 'bus_line_details' to 'bus_lines'.

-- Deletes duplicates by keeping the row with the smallest bus_line_details_id (or another unique column)
DELETE FROM bus_line_details
WHERE ctid NOT IN (
  SELECT MIN(ctid)
  FROM bus_line_details
  GROUP BY bus_station_ID, bus_line_ID
);


ALTER TABLE bus_line_details
    ADD PRIMARY KEY (bus_station_ID, bus_line_ID);
-- Remove 'bus_station_name' and 'bus_line_name' if these are no longer needed
ALTER TABLE bus_line_details
    DROP COLUMN IF EXISTS bus_station_name,
    DROP COLUMN IF EXISTS bus_line_name;



-- Add foreign key constraints
ALTER TABLE bus_line_details
    ADD CONSTRAINT fk_bus_line_details_bus_station
    FOREIGN KEY (bus_station_ID) REFERENCES bus_stations(bus_station_ID),
    ADD CONSTRAINT fk_bus_line_details_bus_line
    FOREIGN KEY (bus_line_ID) REFERENCES bus_lines(bus_lines_ID);

-- -- Add a 'passenger_ID' column to 'passengers' and set it as the primary key
-- Alter table passengers drop constraint passengers_pkey cascade;
-- ALTER TABLE passengers
--     ADD COLUMN IF NOT EXISTS passenger_ID BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY;
--

-- Drop table identifier cascade;

-- Step 1: Create the 'identifier' table with an ID and a type column
CREATE TABLE IF NOT EXISTS identifier (
    ID varchar(50) PRIMARY KEY,
    type VARCHAR(50)
);

-- Step 2: Update the 'identifier' table with distinct passenger IDs
INSERT INTO identifier (ID, type)
SELECT DISTINCT id_number, 'passenger'
FROM passengers
ON CONFLICT DO NOTHING; -- Handle duplicates gracefully

-- Step 3: Update the 'identifier' table with distinct card IDs
INSERT INTO identifier (ID, type)
SELECT DISTINCT code, 'card'
FROM cards
ON CONFLICT DO NOTHING; -- Handle duplicates gracefully

-- Step 4: Create foreign keys to link 'identifier' with 'passengers' and 'cards' tables
ALTER TABLE passengers
ADD CONSTRAINT fk_passenger_identifier
FOREIGN KEY (id_number)
REFERENCES identifier (ID);

ALTER TABLE cards
ADD CONSTRAINT fk_card_identifier
FOREIGN KEY (code)
REFERENCES identifier (ID);

ALTER TABLE rides
    ADD CONSTRAINT fk_rides_rail_user FOREIGN KEY (rail_user) REFERENCES identifier(ID);