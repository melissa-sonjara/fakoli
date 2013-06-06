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
) ENGINE=InnoDB;

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
) ENGINE=InnoDB;

CREATE TABLE `calendar_event_type_xref` (
  `calendar_event_type_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `calendar_id` int(10) unsigned NOT NULL DEFAULT '0',
  `event_type_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`calendar_event_type_xref_id`)
) ENGINE=InnoDB;

-- END Version 1.1

-- START Version 1.2

-- JDG 10/27/2011 removed event types
-- Fakoli should not have any data specific to just one of our websites.
 
--END Version 1.2
 
-- START Version 1.3


CREATE TABLE  `event_invitation` (
  `event_invitation_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `sender_email` varchar(40) NOT NULL,
  `sender_name` varchar(100) NOT NULL,
  `subject` varchar(150) NOT NULL,
  `message` text,
  `sequence_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`event_invitation_id`),
  KEY `event_idx` (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `event` change column `start_date` `start_date` DATETIME DEFAULT NULL;
ALTER TABLE `event` change column `end_date` `end_date` DATETIME DEFAULT NULL;
ALTER TABLE `event` add column `event_invitation_id` int(10) NOT NULL DEFAULT '0';

ALTER TABLE `event` ADD INDEX `event_invitation_idx`(`event_invitation_id`);

-- END Version 1.3

-- START Version 1.4

alter table `event` add column `allow_access` varchar(200) NOT NULL DEFAULT '' AFTER `event_type`;

-- END Version 1.4

-- START Version 1.5

CREATE TABLE `time_zone` (
  `time_zone_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `time_zone_name` varchar(80) NOT NULL,
  `standard_offset` double NOT NULL default '0',
  `daylight_offset` double NOT NULL default '0',
  PRIMARY KEY (`time_zone_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `time_zone` (`time_zone_name`,`standard_offset`,`daylight_offset`) VALUES 
 ('US/Eastern',-5,-4),
 ('US/Pacific',-8,-7),
 ('US/Mountain',-7,-6),
 ('US/Indiana-Starke',-6,-5),
 ('US/East-Indiana',-5,-4),
 ('US/Central',-6,-5),
 ('US/Arizona',-7,-7),
 ('US/Hawaii',-10,-10),
 ('US/Alaska',-9,-8);

alter table `event` add column `time_zone_id` int(10) NOT NULL DEFAULT '0' AFTER `end_date`;

-- END Version 1.5

-- START Version 1.6

alter table `event` add column `owner_id` int(10) NOT NULL DEFAULT '0' AFTER `allow_access`;


-- END Version 1.6
