-- Fakoli Settings Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- The initial create statement for settings is in component_schema.sql 
-- Version 0 - Bootstrap

-- START Version 1.0
-- settings table must be created before scan
-- END Version 1.0

-- START Version 1.1
-- 

update `settings` set `component` = 'debug', `category` = '' where `name` like 'trace_%';
 
-- 
-- END Version 1.1


-- START Version 1.2

ALTER TABLE `settings` change column `value` `value` varchar(2000) NOT NULL;

-- END Version 1.2

-- START Version 1.3

delete from `settings` where `name` = 'google_analytics';
ALTER TABLE `settings` change column `value` `value` varchar(500) NOT NULL;

-- END Version 1.3
