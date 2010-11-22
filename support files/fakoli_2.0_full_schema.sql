DROP TABLE IF EXISTS `admin_page`;
CREATE TABLE  `admin_page` (
  `admin_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`admin_page_id`),
  KEY `admin_page_identifier_idx` (`identifier`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `article`;
CREATE TABLE  `article` (
  `article_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(200) NOT NULL default '',
  `message` longtext NOT NULL,
  `last_modified` datetime default NULL,
  `published` tinyint(3) unsigned NOT NULL default '0',
  `image_id` int(10) unsigned default '0',
  `teaser` text NOT NULL,
  `archive_date` date default NULL,
  `created_date` datetime default NULL,
  `article_type` varchar(200) NOT NULL default '',
  `allow_comment` tinyint(3) default '0',
  `headline` tinyint(3) default '0',
  `author_id` int(10) unsigned NOT NULL default '0',
  `tags` varchar(400) NOT NULL default '',
  PRIMARY KEY  (`article_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

DROP TABLE IF EXISTS `article_comment_xref`;
CREATE TABLE  `article_comment_xref` (
  `article_comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned DEFAULT NULL,
  `comment_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`article_comment_id`) 
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `article_site_xref`;
CREATE TABLE  `article_site_xref` (
  `article_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `article_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`article_site_xref_id`) 
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `attachment`;
CREATE TABLE  `attachment` (
  `attachment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `filename` varchar(200) DEFAULT NULL,
  `local_filename` varchar(200) DEFAULT NULL,
  `last_modified` datetime DEFAULT NULL,
  `file_size` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`attachment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `blog`;
CREATE TABLE  `blog` (
  `blog_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(200) NOT NULL,
  `description` text,
  `owner_id` int(10) unsigned NOT NULL default '0',
  `read_access` varchar(400) NOT NULL default '',
  `write_access` varchar(400) NOT NULL default '',
  `published` tinyint(3) unsigned NOT NULL default '0',
  `created_date` datetime NOT NULL,
  `identifier` varchar(100) NOT NULL,
  `allow_comments` tinyint(3) unsigned NOT NULL default '0',
  `default_article_order` varchar(4) NOT NULL default 'DESC',
  `image_id` int(10) unsigned NOT NULL default '0',
  `blog_type` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`blog_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `calendar`;
CREATE TABLE  `calendar` (
  `calendar_id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(100) default NULL,
  `write_access` varchar(100) default NULL,
  `identifier` varchar(100) NOT NULL,
  PRIMARY KEY  (`calendar_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE  `comment` (
  `comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text,
  `date_posted` datetime NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `user_id` int(10) unsigned DEFAULT NULL,
  `author` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`comment_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;



DROP TABLE IF EXISTS `contact_topic`;
CREATE TABLE  `contact_topic` (
  `contact_topic_id` int(10) unsigned NOT NULL auto_increment,
  `topic` varchar(100) NOT NULL,
  `recipients` text COMMENT 'one or more admin users to receive email',
  `sort_order` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`contact_topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Topics for Contact Us form';

INSERT INTO  `contact_topic` (`topic`,`sort_order`) VALUES
("Website Question/Comment", 1),
("Other", 2);

DROP TABLE IF EXISTS `contact_topic_site_xref`;
CREATE TABLE  `contact_topic_site_xref` (
  `contact_topic_site_xref_id` int(10) unsigned NOT NULL auto_increment,
  `contact_topic_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`contact_topic_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `component`;
CREATE TABLE  `component` (
  `component_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `version` varchar(25) NOT NULL,
  `priority` int(10) unsigned NOT NULL DEFAULT '0',
  `enabled` int(10) unsigned NOT NULL,
  `author` varchar(45) NOT NULL DEFAULT '',
  `description` varchar(300) NOT NULL DEFAULT '',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `component_path` varchar(300) NOT NULL,
  PRIMARY KEY (`component_id`),
  KEY `components_by_name` (`name`,`enabled`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `component_page`;
CREATE TABLE  `component_page` (
  `component_page_id` int(10) unsigned NOT NULL auto_increment,
  `identifier` varchar(50) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component` varchar(50) NOT NULL,
  `role` varchar(100) NOT NULL default '',
  `enabled` tinyint(3) unsigned NOT NULL default '0',
  `scanned` tinyint(3) unsigned NOT NULL default '0',
  `template` varchar(100) NOT NULL default '',
  `page_title` varchar(200) NOT NULL,
  `site_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`component_page_id`),
  KEY `component_page_idx` (`identifier`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


DROP TABLE IF EXISTS `component_page_module_xref`;
CREATE TABLE  `component_page_module_xref` (
  `join_id` int(10) unsigned NOT NULL auto_increment,
  `component_page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`join_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `contact_topic`;
CREATE TABLE  `contact_topic` (
  `contact_topic_id` int(10) unsigned NOT NULL auto_increment,
  `topic` varchar(100) NOT NULL,
  `recipients` text COMMENT 'one or more admin users to receive email',
  `site_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`contact_topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Topics for Contact Us form';

DROP TABLE IF EXISTS `context_help`;
CREATE TABLE  `context_help` (
  `help_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `class_name` varchar(100) NOT NULL,
  `field` varchar(100) NOT NULL,
  `help` text,
  `criteria` text,
  `title` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`help_id`),
  KEY `help_by_class_idx` (`class_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `document`;
CREATE TABLE  `document` (
  `document_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(1000) NOT NULL,
  `publication_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `keywords` varchar(1000) DEFAULT NULL,
  `notes` text,
  `public` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `file` varchar(400) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `document_folder`;
CREATE TABLE  `document_folder` (
  `folder_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `path` varchar(255) NOT NULL,
  `permissions` varchar(100) NOT NULL,
  `document_library_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`folder_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `document_library`;
CREATE TABLE  `document_library` (
  `document_library_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `manage_folders` varchar(200) NOT NULL DEFAULT '',
  `upload_files` varchar(200) NOT NULL DEFAULT '',
  `delete_files` varchar(200) NOT NULL DEFAULT '',
  `last_modified` datetime NOT NULL,
  `allow_access` varchar(200) NOT NULL,
  `identifier` varchar(100) NOT NULL,
  PRIMARY KEY (`document_library_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `document_topic_xref`;
CREATE TABLE  `document_topic_xref` (
  `document_topic_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `document_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `group_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`document_topic_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `email_template`;
CREATE TABLE  `email_template` (
  `email_template_id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(100) NOT NULL COMMENT 'title of the message type',
  `sender_email` varchar(100) default NULL,
  `recipients` text,
  `subject` varchar(100) NOT NULL COMMENT 'email subject line',
  `message` text NOT NULL COMMENT 'text of the message',
  `class_name` varchar(100) NOT NULL,
  PRIMARY KEY  (`email_template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC COMMENT='email messages';


DROP TABLE IF EXISTS `event`;
CREATE TABLE  `event` (
  `event_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(255) NOT NULL DEFAULT '',
  `start_date` datetime DEFAULT NULL,
  `end_date` datetime DEFAULT NULL,
  `location` varchar(255) NOT NULL DEFAULT '',
  `description` text NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_modified` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `event_type` varchar(200) NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  `calendar_id` int(10) unsigned DEFAULT '0',
  PRIMARY KEY (`event_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `event_site_xref`;
CREATE TABLE  `event_site_xref` (
  `event_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`event_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `forum`;
CREATE TABLE  `forum` (
  `forum_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text NOT NULL,
  `created_date` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `title` varchar(200) NOT NULL,
  `teaser` varchar(300) NOT NULL,
  PRIMARY KEY (`forum_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_message`;
CREATE TABLE  `forum_message` (
  `message_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(300) NOT NULL,
  `message` text NOT NULL,
  `parent_id` int(10) unsigned NOT NULL DEFAULT '0',
  `topic_id` int(10) unsigned NOT NULL DEFAULT '0',
  `author_id` int(10) unsigned NOT NULL,
  `date_posted` datetime NOT NULL,
  `last_modified` datetime NOT NULL,
  `forum_id` int(10) unsigned NOT NULL DEFAULT '0',
  `deleted` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`message_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_message_attachment_xref`;
CREATE TABLE  `forum_message_attachment_xref` (
  `forum_message_attachment_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `attachment_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_message_attachment_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_site_xref`;
CREATE TABLE  `forum_site_xref` (
  `forum_site_xref_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `forum_id` int(10) unsigned NOT NULL,
  `site_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`forum_site_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `forum_subscription`;
CREATE TABLE  `forum_subscription` (
  `forum_subscription_id` int(10) unsigned NOT NULL auto_increment,
  `forum_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `subscription_type` tinyint(3) unsigned NOT NULL COMMENT 'daily digest or new item notification',
  PRIMARY KEY  (`forum_subscription_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Links forums and forum topics to subscribed users';


DROP TABLE IF EXISTS `forum_topic`;
CREATE TABLE  `forum_topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `message_id` int(10) unsigned NOT NULL,
  `views` int(10) unsigned NOT NULL DEFAULT '0',
  `replies` int(10) unsigned NOT NULL DEFAULT '0',
  `forum_id` int(10) unsigned NOT NULL,
  `date_created` datetime NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `grp`;
CREATE TABLE  `grp` (
  `group_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) DEFAULT NULL,
  `Description` text NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `grp_topic_xref`;
CREATE TABLE  `grp_topic_xref` (
  `group_topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `group_id` int(10) unsigned NOT NULL,
  `topic_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`group_topic_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `image`;
CREATE TABLE  `image` (
  `image_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(200) NOT NULL,
  `ALT_tag` varchar(100) NOT NULL,
  `date_taken` date default NULL,
  `credits` varchar(200) NOT NULL default '',
  `caption` varchar(200) NOT NULL default '',
  `keywords` varchar(200) NOT NULL default '',
  `image_file` varchar(200) NOT NULL default '',
  `include_in_slideshow` tinyint(3) unsigned NOT NULL default '0',
  `image_type` varchar(50) NOT NULL default '',
  `gallery_id` int(10) unsigned NOT NULL default '0',
  `published` tinyint(3) unsigned NOT NULL default '0',
  `owner_id` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`image_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `image_gallery`;
CREATE TABLE  `image_gallery` (
  `gallery_id` int(10) unsigned NOT NULL auto_increment,
  `gallery_name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL default '0',
  `keywords` varchar(400) default NULL,
  `identifier` varchar(100) default NULL,
  `last_modified` datetime NOT NULL,
  `read_access` varchar(500) default NULL,
  `write_access` varchar(500) default NULL,
  `published` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`gallery_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

DROP TABLE IF EXISTS `login_token`;
CREATE TABLE  `login_token` (
  `token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `token` varchar(10) NOT NULL,
  `expire_date` date NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`token_id`) ,
  UNIQUE KEY `token_idx` (`token`) 
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `menu`;
CREATE TABLE  `menu` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_id` int(10) unsigned NOT NULL,
  `name` varchar(200) NOT NULL,
  `identifier` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `css_class` varchar(100) NOT NULL,
  PRIMARY KEY (`menu_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `menu_item`;
CREATE TABLE  `menu_item` (
  `menu_item_id` int(10) unsigned NOT NULL auto_increment,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) default NULL,
  `published` tinyint(3) unsigned default NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `role` varchar(100) NOT NULL default '',
  `url` varchar(200) NOT NULL,
  `menu_id` int(10) unsigned NOT NULL,
  `identifier` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`menu_item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


DROP TABLE IF EXISTS `merge_code`;
CREATE TABLE  `merge_code` (
  `merge_code_id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(45) NOT NULL COMMENT 'name used in the template',
  `description` text,
  `function` tinyint(1) NOT NULL default '0' COMMENT 'whether value_field is a function',
  `map` varchar(80) default NULL COMMENT 'how the code maps through relations to the value',
  `class_name` varchar(50) NOT NULL COMMENT 'class name that can map this code',
  PRIMARY KEY  USING BTREE (`merge_code_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC COMMENT='email msg map field names to their values';

-- codes for survey feature
INSERT INTO `merge_code` (`name`,`description`,`function`,`map`,`class_name`) VALUES

 ('survey_link','Inserts a link to the survey',1,'getEditUrl','SurveyResponse'),
 ('survey_intro_link','Inserts a link to the survey, starting on its landing page.',1,'getIntroUrl','SurveyResponse'),
 ('token','The key that enables a specific user to access the survey. This is generated randomly by the system when you send out the survey emails.',0,'SurveyResponse.token','SurveyResponse'),
 ('survey_title','The title of the survey.',0,'Survey.title','SurveyResponse'),
 ('survey_end_date','The last day that the survey will accept responses. Use only if you have specified a survey end date.',0,'Survey.end_date','SurveyResponse');


DROP TABLE IF EXISTS `module`;
CREATE TABLE  `module` (
  `module_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `group_id` int(10) unsigned NOT NULL DEFAULT '0',
  `content_type` varchar(100) DEFAULT NULL,
  `num_items` int(10) unsigned NOT NULL DEFAULT '5',
  `ord_by` varchar(100) DEFAULT NULL,
  `template` text,
  `query_constraint` varchar(200) DEFAULT NULL,
  `php_code_file` varchar(200) DEFAULT NULL,
  `css_class` varchar(200) DEFAULT NULL,
  `menu_id` int(10) unsigned NOT NULL DEFAULT '0',
  `menu_parameters` varchar(200) DEFAULT NULL,
  `global` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `global_position` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`module_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;


DROP TABLE IF EXISTS `page`;
CREATE TABLE  `page` (
  `page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_title` varchar(200) DEFAULT NULL,
  `description` text,
  `author` varchar(45) NOT NULL,
  `created_date` date NOT NULL,
  `edited_date` date NOT NULL,
  `meta_tag_description` varchar(200) DEFAULT NULL,
  `meta_tag_keyword` varchar(200) DEFAULT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `published` tinyint(3) unsigned NOT NULL,
  `template` varchar(100) DEFAULT NULL,
  `identifier` varchar(45) NOT NULL,
  `php_code_file` varchar(100) NOT NULL DEFAULT '',
  `site_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`page_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `page_module_xref`;
CREATE TABLE  `page_module_xref` (
  `join_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `page_id` int(10) unsigned NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `position` varchar(45) NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  PRIMARY KEY (`join_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `question_type`;
CREATE TABLE  `question_type` (
  `question_type_id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(45) NOT NULL COMMENT 'type name',
  `class_name` varchar(100) NOT NULL COMMENT 'the name of the question renderer class',
  `sort_order` tinyint(3) unsigned NOT NULL default '0' COMMENT 'order in the question form drop down',
  `options` tinyint(3) unsigned NOT NULL default '0' COMMENT 'whether this type has an options list',
  `char_limit` tinyint(3) unsigned NOT NULL default '0' COMMENT 'whether this type can have a char limit; 0 if not',
  `num_rows` tinyint(3) unsigned NOT NULL default '0' COMMENT 'default number of rows, if applicable to type; 0 if not',
  PRIMARY KEY  USING BTREE (`question_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Question types available for questionnaires';


/*!40000 ALTER TABLE `question_type` DISABLE KEYS */;
INSERT INTO `question_type` (`question_type_id`,`name`,`class_name`,`sort_order`,`options`,`char_limit`,`num_rows`) VALUES 
 (1,'Multiple Choice','MultipleChoiceView',4,1,0,0),
 (2,'Rating Question','RatingView',5,1,0,0),
 (3,'Short Text','ShortTextView',2,0,0,0),
 (4,'Free Text','FreeTextView',1,0,1,3),
 (5,'Checklist','CheckListView',3,1,0,0),
 (6,'Drop Down List','SelectFieldView',6,1,0,0);
/*!40000 ALTER TABLE `question_type` ENABLE KEYS */;


DROP TABLE IF EXISTS `site`;
CREATE TABLE  `site` (
  `site_id` int(10) unsigned NOT NULL auto_increment,
  `site_name` varchar(200) NOT NULL,
  `domain` varchar(200) NOT NULL default '',
  `enabled` tinyint(3) unsigned NOT NULL default '0',
  `description` varchar(300) NOT NULL default '',
  `default_template` varchar(200) NOT NULL,
  `print_template` varchar(200) default NULL,
  `popup_template` varchar(200) default NULL,
  `mobile_template` varchar(200) default NULL,
  `home_page` varchar(200) default NULL,
  `default_site` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`site_id`,`description`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

DROP TABLE IF EXISTS `site_map`;
CREATE TABLE  `site_map` (
  `site_map_id` int(10) unsigned NOT NULL auto_increment,
  `identifier` varchar(100) default NULL,
  `page_title` varchar(200) NOT NULL default '',
  `sort_order` tinyint(3) unsigned NOT NULL default '0',
  `role` varchar(100) NOT NULL default '',
  `parent_identifier` varchar(100) default NULL,
  `published` tinyint(3) unsigned default '0' COMMENT 'whether to display this page in a site map view',
  PRIMARY KEY  USING BTREE (`site_map_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='defines the site hierarchy';

DROP TABLE IF EXISTS `site_role`;
CREATE TABLE  `site_role` (
  `role_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `role` varchar(45) NOT NULL,
  `name` varchar(45) NOT NULL,
  `description` text,
  PRIMARY KEY (`role_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `site_user`;
CREATE TABLE  `site_user` (
  `user_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `first_name` varchar(100) NOT NULL,
  `last_name` varchar(100) NOT NULL,
  `role` varchar(20) NOT NULL,
  `title` varchar(100) NOT NULL,
  `username` varchar(100) NOT NULL,
  `password` varchar(100) NOT NULL,
  `email` varchar(200) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL,
  `notes` text NOT NULL,
  `composite_class` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `topic`;
CREATE TABLE  `topic` (
  `topic_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `topic` varchar(100) NOT NULL,
  PRIMARY KEY (`topic_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
-- Tables for Questionnaire
--

DROP TABLE IF EXISTS `answer`;
CREATE TABLE  `answer` (
  `answer_id` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY  (`answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

DROP TABLE IF EXISTS `question`;
CREATE TABLE  `question` (
  `question_id` int(10) unsigned NOT NULL auto_increment,
  `questionnaire_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY  (`question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';

DROP TABLE IF EXISTS `question_type`;
CREATE TABLE  `question_type` (
  `question_type_id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(45) NOT NULL COMMENT 'type name',
  `class_name` varchar(100) NOT NULL COMMENT 'the name of the question renderer class',
  `sort_order` tinyint(3) unsigned NOT NULL default '0' COMMENT 'order in the question form drop down',
  `options` tinyint(3) unsigned NOT NULL default '0' COMMENT 'whether this type has an options list',
  `char_limit` tinyint(3) unsigned NOT NULL default '0' COMMENT 'whether this type can have a char limit; 0 if not',
  `num_rows` tinyint(3) unsigned NOT NULL default '0' COMMENT 'default number of rows, if applicable to type; 0 if not',
  PRIMARY KEY  USING BTREE (`question_type_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Question types available for questionnaires';

DROP TABLE IF EXISTS `questionnaire`;
CREATE TABLE  `questionnaire` (
  `questionnaire_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  PRIMARY KEY  (`questionnaire_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='links a questionnaire to its container class';

-- * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
-- Tables for Survey Component
--

DROP TABLE IF EXISTS `survey`;
CREATE TABLE  `survey` (
  `survey_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `start_date` date default NULL COMMENT 'date survey request sent',
  `end_date` date default NULL COMMENT 'date survey is closed',
  `user_id` int(10) unsigned NOT NULL COMMENT 'references site_user table',
  `sender_email` varchar(100) default NULL,
  `recipients` text,
  `message` text,
  `status` tinyint(3) unsigned NOT NULL default '0',
  `deleted` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Links a questionnaire to an email template';

DROP TABLE IF EXISTS `survey_answer`;
CREATE TABLE  `survey_answer` (
  `survey_answer_id` int(10) unsigned NOT NULL auto_increment,
  `response_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire_response table',
  `survey_question_id` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY  USING BTREE (`survey_answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';


DROP TABLE IF EXISTS `survey_question`;
CREATE TABLE  `survey_question` (
  `survey_question_id` int(10) unsigned NOT NULL auto_increment,
  `question_type_id` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned default '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned default '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned default '0' COMMENT 'number of input rows to display on freetext',
  `locked` tinyint(3) unsigned NOT NULL default '0' COMMENT 'whether this is a standard, multi-reuse question',
  PRIMARY KEY  USING BTREE (`survey_question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';


DROP TABLE IF EXISTS `survey_question_xref`;
CREATE TABLE  `survey_question_xref` (
  `survey_question_xref_id` int(10) unsigned NOT NULL auto_increment,
  `survey_id` int(10) unsigned default NULL COMMENT 'if survey question set',
  `survey_template_id` int(10) unsigned default NULL COMMENT 'if standard question set',
  `survey_question_id` int(10) unsigned NOT NULL,
  `sort_order` tinyint(4) unsigned NOT NULL default '0',
  PRIMARY KEY  (`survey_question_xref_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='survey question set or standard set';


DROP TABLE IF EXISTS `survey_response`;
CREATE TABLE  `survey_response` (
  `response_id` int(10) unsigned NOT NULL auto_increment,
  `survey_id` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `token` varchar(10) NOT NULL COMMENT 'assigned response token',
  `email` varchar(100) NOT NULL,
  `last_modified` date default NULL COMMENT 'date submitted',
  `status` tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  (`response_id`),
  UNIQUE KEY `token_idx` (`token`,`survey_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `survey_setting`;
CREATE TABLE  `survey_setting` (
  `survey_setting_id` int(10) unsigned NOT NULL auto_increment,
  `sender_email` varchar(100) NOT NULL,
  `email_subject` varchar(200) NOT NULL,
  `message_footer` text NOT NULL,
  PRIMARY KEY  (`survey_setting_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='setting and options or survey email messages';


DROP TABLE IF EXISTS `survey_template`;
CREATE TABLE  .`survey_template` (
  `survey_template_id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(100) NOT NULL,
  `introduction` text COMMENT 'instructions to users completing survey',
  `message` text NOT NULL,
  `status` tinyint(3) unsigned NOT NULL default '0' COMMENT 'in progress, published, in review',
  PRIMARY KEY  USING BTREE (`survey_template_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- end survey tables




DROP VIEW IF EXISTS `forum_summary`;
CREATE VIEW `forum_summary` AS 
select `f`.`forum_id` AS `forum_id`,
`f`.`title` AS `title`,
`f`.`teaser` AS `teaser`,
`f`.`published` AS `published`,
(select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;