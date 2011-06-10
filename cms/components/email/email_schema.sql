-- Fakoli Email Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `contact_topic` (
  `contact_topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  `recipients` text COMMENT 'one or more admin users to receive email',
  `sort_order` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`contact_topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Topics for Contact Us form';

CREATE TABLE `contact_topic_site_xref` (
  `contact_topic_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `contact_topic_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`contact_topic_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `email_template` (
  `email_template_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL COMMENT 'title of the message type',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `subject` varchar(100) NOT NULL COMMENT 'email subject line',
  `message` text NOT NULL COMMENT 'text of the message',
  `class_name` varchar(100) NOT NULL,
  PRIMARY KEY (`email_template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC COMMENT='email messages';

CREATE TABLE `merge_code` (
  `merge_code_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'name used in the template',
  `description` text,
  `function` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'whether value_field is a function',
  `map` varchar(80) DEFAULT NULL COMMENT 'how the code maps through relations to the value',
  `class_name` varchar(50) NOT NULL COMMENT 'class name that can map this code',
  PRIMARY KEY (`merge_code_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC COMMENT='email msg map field names to their values';


-- END Version 1.0
