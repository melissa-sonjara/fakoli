-- Fakoli Login Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `login_token` (
  `token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(10) NOT NULL,
  `expire_date` date NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`token_id`),
  UNIQUE KEY `token_idx` (`token`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
