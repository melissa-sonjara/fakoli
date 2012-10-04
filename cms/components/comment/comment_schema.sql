-- Fakoli Comment Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `comment` (
  `comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text,
  `date_posted` datetime NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `user_id` int(10) unsigned DEFAULT NULL,
  `author` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`comment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `comment` ADD COLUMN `published` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0 AFTER `author`;

-- END Version 1.1