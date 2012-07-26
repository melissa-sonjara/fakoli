-- Fakoli Context Help Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `context_help` (
  `help_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `class_name` varchar(100) NOT NULL,
  `field` varchar(100) NOT NULL,
  `help` text,
  `criteria` text,
  `title` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`help_id`),
  KEY `help_by_class_idx` (`class_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
