-- Fakoli Auto Login Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `authentication_token` (
  `authentication_token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(200) NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `ip_address` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`authentication_token_id`)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `authentication_token` ADD COLUMN `single_use` tinyint(3) UNSIGNED NOT NULL DEFAULT '0';
ALTER TABLE `authentication_token` ADD COLUMN `last_access` DATETIME NULL;
ALTER TABLE `authentication_token` ADD COLUMN `redirect_on_success` VARCHAR(255) NULL;

-- END Version 1.1