
DROP TABLE IF EXISTS `document`;
CREATE TABLE  `document` (
  `document_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(1000) NOT NULL,
  `publication_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `keywords` varchar(1000) DEFAULT NULL,
  `abstract` text,
  `public` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `file` varchar(400) NOT NULL,
  `is_local` tinyint(3) unsigned zerofill NOT NULL DEFAULT '000',
  `author` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`document_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `document_topic_xref`;
CREATE TABLE  `document_topic_xref` (
  `document_topic_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `group_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_topic_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `event`;
CREATE TABLE  `event` (
  `event_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL DEFAULT '',
  `start_date` date DEFAULT NULL,
  `end_date` date DEFAULT NULL,
  `location` varchar(255) NOT NULL DEFAULT '',
  `description` text NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `event_type` varchar(200) NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `event_site_xref`;
CREATE TABLE  `event_site_xref` (
  `event_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`event_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;