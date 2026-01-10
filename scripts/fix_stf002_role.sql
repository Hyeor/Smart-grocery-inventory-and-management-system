-- Fix STF002 role to STAFF
UPDATE User SET role = 'STAFF' WHERE staff_id = 'STF002' AND (role IS NULL OR role = '');

-- Fix STF002 full name by trimming any leading/trailing spaces
UPDATE User SET full_name = TRIM(full_name) WHERE staff_id = 'STF002';
