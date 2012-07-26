-- Fakoli Settings Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0
-- settings table must be created before scan
-- END Version 1.0

-- START Version 1.1
-- 

update `settings` set `component` = 'debug', `category` = '' where `name` like 'trace_%';
 
-- 
-- END Version 1.1

