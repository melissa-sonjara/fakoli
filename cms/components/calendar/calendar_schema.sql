-- Fakoli Calendar Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `calendar` (
  `calendar_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(100) DEFAULT NULL,
  `write_access` varchar(100) DEFAULT NULL,
  `identifier` varchar(100) NOT NULL,
  PRIMARY KEY (`calendar_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `event` (
  `event_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL DEFAULT '',
  `start_date` datetime DEFAULT NULL,
  `end_date` datetime DEFAULT NULL,
  `location` varchar(255) NOT NULL DEFAULT '',
  `description` text NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `event_type` varchar(200) NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  `calendar_id` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `event_site_xref` (
  `event_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`event_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- END Version 1.0

-- START Version 1.1

CREATE TABLE  `event_type` (
  `event_type_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  PRIMARY KEY (`event_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `calendar_event_type_xref` (
  `calendar_event_type_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `calendar_id` int(10) unsigned NOT NULL DEFAULT '0',
  `event_type_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`calendar_event_type_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.1

-- START Version 1.2

INSERT INTO `event_type` (`event_type_id`,`name`) VALUES 
 (1,'Competition'),
 (2,'Mission'),
 (3,'Kick-off'),
 (4,'Showcase'),
 (5,'Training');
 
 --END Version 1.2
