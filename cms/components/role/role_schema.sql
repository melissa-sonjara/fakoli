-- Fakoli Role Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0
-- create role table and insert record before scan
-- END Version 1.0

-- START Version 1.1

ALTER TABLE site_role ADD COLUMN priority int(10) not null default 1;
ALTER TABLE site_role ADD COLUMN home_page VARCHAR(500) not null default '';

-- END Version 1.1