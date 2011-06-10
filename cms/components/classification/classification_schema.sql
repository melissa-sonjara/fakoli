-- Fakoli Classification Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `grp` (
  `group_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) DEFAULT NULL,
  `Description` text NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `document_topic_xref` (
  `document_topic_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `group_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_topic_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `grp_topic_xref` (
  `group_topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `group_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`group_topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- END Version 1.0
