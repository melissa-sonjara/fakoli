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
) ENGINE=InnoDB COMMENT='Topics for Contact Us form';

CREATE TABLE `contact_topic_site_xref` (
  `contact_topic_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `contact_topic_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`contact_topic_site_xref_id`)
) ENGINE=InnoDB;

CREATE TABLE `email_template` (
  `email_template_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL COMMENT 'title of the message type',
  `sender_email` varchar(100) DEFAULT NULL,
  `recipients` text,
  `subject` varchar(100) NOT NULL COMMENT 'email subject line',
  `message` text NOT NULL COMMENT 'text of the message',
  `class_name` varchar(100) NOT NULL,
  PRIMARY KEY (`email_template_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC COMMENT='email messages';


INSERT INTO `email_template` (`name`,`sender_email`,`recipients`,`subject`,`message`,`class_name`) VALUES 
  ('test','','[email]','test [field]','relation and field:<br>[relation_field]<br><br>field of sending class:<br>[field]<br><br>callback of a class<br>[class_field]<br><br>function in sending class<br>[function]<br><br>function in relation<br>[relation_function]<br><br>callback function not in a class<br>[callback_function]<br><br>test invalid code<br>[invalid_code]<br><br>test invalid no map<br>[invalid_no_map]<br><br>test field with no merge code record<br>[string]<br><br>test merge code with html inside- email manager should strip the html so that the code is valid.<br>[<span>field]</span><br><br><br>test no merge code record on relation<br>[EmailManagerTestRelation.string]<br><br>test date field<br><br>[<span>date]</span>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n','EmailManagerTest');

DROP TABLE IF EXISTS `merge_code`;
CREATE TABLE  `merge_code` (
  `merge_code_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL COMMENT 'name used in the template',
  `description` text,
  `function` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'whether value_field is a function',
  `map` varchar(80) DEFAULT NULL COMMENT 'how the code maps through relations to the value',
  `class_name` varchar(50) NOT NULL COMMENT 'class name that can map this code',
  PRIMARY KEY (`merge_code_id`)
) ENGINE=InnoDB ROW_FORMAT=DYNAMIC COMMENT='email msg map field names to their values';

INSERT INTO `merge_code` (`name`,`function`,`map`,`description`,`class_name`) VALUES
('relation_field',0,'EmailManagerTestRelation.string','','EmailManagerTest'),
 ('field',0,'string','','EmailManagerTest'),
 ('class_field',1,'EmailManagerTestClass.TestCallback','','EmailManagerTest'),
 ('function',1,'TestFunction','','EmailManagerTest'),
 ('relation_function',1,'EmailManagerTestRelation.TestRelationFunction','','EmailManagerTest'),
 ('callback_function',1,'emailManagerCallbackFunction','','EmailManagerTest'),
 ('invalid_code',0,'nothing','','EmailManagerTest'),
 ('invalid_no_map',0,'','','EmailManagerTest'),
 ('merge_code_not_used',0,'test','test merge code not used in template','EmailManagerTest');


DROP TABLE IF EXISTS `email_manager_test`;
CREATE TABLE `email_manager_test` (
  `test_id` int(10) unsigned NOT NULL auto_increment,
  `string` varchar(20) default NULL,
  `relation_id` int(10) unsigned default NULL,
  `date` date default NULL,
  `email` varchar(50) default NULL,
  PRIMARY KEY  USING BTREE (`test_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 ROW_FORMAT=DYNAMIC;

INSERT INTO `email_manager_test` (`test_id`,`string`,`relation_id`,`date`,`email`) VALUES 
 (1,'test_string',1,'2011-03-12','janice@sonjara.com');

DROP TABLE IF EXISTS `email_manager_test_relation`;
CREATE TABLE `email_manager_test_relation` (
  `relation_id` int(10) unsigned NOT NULL auto_increment,
  `string` varchar(20) default NULL,
  PRIMARY KEY  USING BTREE (`relation_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 ROW_FORMAT=DYNAMIC;


INSERT INTO `email_manager_test_relation` (`relation_id`,`string`) VALUES
 (1,'test_relation_string');


-- END Version 1.0

-- START Version 1.1

CREATE TABLE contact_us (
	contact_us_id		int(10) unsigned not null auto_increment,
	full_name			varchar(1000) not null,
	email				varchar(300) not null,
	contact_topic_id	int(10) unsigned not null default 0,
	subject				varchar(1000),
	message				LONGTEXT,
	PRIMARY KEY(contact_us_id)
) ENGINE=InnoDB;

-- AJG: Moved into 1.1 to resolve update issues due to a typo in a previous version of this schema
ALTER TABLE contact_us ADD COLUMN date_sent DATETIME NULL;

-- END Version 1.1

-- START Version 1.2


-- END Version 1.2

-- START Version 1.3

CREATE TABLE email_log
(
	email_log_id	int(10) not null auto_increment,
	sender_name		varchar(255) not null default '',
	sender_email	varchar(255) not null default '',
	recipient_name	varchar(255) not null default '',
	recipient_email	varchar(255) not null default '',
	subject			varchar(1000) not null default '',
	message			longtext,
	date_sent		timestamp,
	PRIMARY KEY(email_log_id)
) ENGINE=InnoDB;

ALTER TABLE email_log ADD INDEX date_idx (date_sent);

-- END Version 1.3