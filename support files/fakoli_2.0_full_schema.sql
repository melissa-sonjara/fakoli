DROP TABLE IF EXISTS `admin_page`;
CREATE TABLE  `admin_page` (
  `admin_page_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `identifier` varchar(100) NOT NULL,
  `server_path` varchar(255) NOT NULL,
  `component_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`admin_page_id`),
  KEY `admin_page_identifier_idx` (`identifier`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

DROP TABLE IF EXISTS `answer`;
CREATE TABLE  `answer` (
  `answer_key` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `container_key` int(10) unsigned NOT NULL COMMENT 'references container class table, e.g., application',
  `class_key` int(10) unsigned NOT NULL COMMENT 'references dataitem_class table, identifies the container class',
  `question_key` int(10) unsigned NOT NULL COMMENT 'references single question in a questionnaire table',
  `value` text,
  PRIMARY KEY (`answer_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Answer to a question';

DROP TABLE IF EXISTS `article`;
CREATE TABLE  `article` (
  `article_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL DEFAULT '',
  `message` text NOT NULL,
  `last_modified` datetime DEFAULT NULL,
  `published` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_id` int(10) unsigned DEFAULT '0',
  `teaser` varchar(300) NOT NULL,
  `archive_date` date NOT NULL,
  `created_date` datetime NOT NULL,
  `article_type` varchar(200) NOT NULL DEFAULT '',
  `allow_comment` tinyint(3) DEFAULT '0',
  `headline` tinyint(3) DEFAULT '0',
  PRIMARY KEY (`article_id`) 
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

DROP TABLE IF EXISTS `calendar`;
CREATE TABLE  `calendar` (
  `calendar_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` text NOT NULL,
  `owner_id` int(10) unsigned NOT NULL,
  `last_modified` datetime NOT NULL,
  `role` varchar(100) NOT NULL,
  PRIMARY KEY (`calendar_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DROP TABLE IF EXISTS `comment`;
CREATE TABLE  `comment` (
  `comment_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `description` text,
  `date_posted` datetime NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `user_id` int(10) unsigned DEFAULT NULL,
  `author` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`comment_id`)
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

DROP TABLE IF EXISTS `dataitem_class`;
CREATE TABLE  `dataitem_class` (
  `class_key` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `class_name` varchar(50) NOT NULL COMMENT 'dataitem class name',
  PRIMARY KEY (`class_key`)
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
  `image_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(200) NOT NULL,
  `ALT_tag` varchar(100) NOT NULL,
  `folder` varchar(200) NOT NULL,
  `date_taken` date DEFAULT NULL,
  `credits` varchar(200) NOT NULL,
  `caption` varchar(200) NOT NULL,
  `keywords` varchar(200) NOT NULL,
  `status` varchar(45) DEFAULT NULL,
  `image_file` varchar(200) NOT NULL,
  `thumbnail` varchar(200) NOT NULL,
  `image_rotator` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `image_type` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`image_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

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
  `menu_item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `published` tinyint(3) unsigned DEFAULT NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `role` varchar(100) NOT NULL DEFAULT '',
  `url` varchar(200) NOT NULL,
  `menu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`menu_item_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

DROP TABLE IF EXISTS `menus`;
CREATE TABLE  `menus` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `parent_id` int(10) unsigned NOT NULL,
  `title` varchar(100) DEFAULT NULL,
  `public` tinyint(3) unsigned DEFAULT NULL,
  `page_id` int(10) unsigned NOT NULL,
  `sort_order` int(10) unsigned NOT NULL,
  `menu_type` varchar(100) NOT NULL,
  `url` varchar(200) NOT NULL,
  PRIMARY KEY (`menu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

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

DROP TABLE IF EXISTS `question`;
CREATE TABLE  `question` (
  `question_key` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `questionnaire_key` int(10) unsigned NOT NULL COMMENT 'references questionnaire table',
  `question_number` int(10) unsigned NOT NULL,
  `question_type` int(10) unsigned NOT NULL COMMENT 'freetext, multiple choice, ratings, etc.',
  `question` text NOT NULL COMMENT 'the text of the question',
  `options` text COMMENT 'for checklists or multiple choice, the list of answer options',
  `required` tinyint(3) unsigned DEFAULT '0' COMMENT '1 for required text; count for required number of checks in checkbox list',
  `char_limit` int(8) unsigned DEFAULT '0' COMMENT 'character limit for free text fields',
  `num_rows` tinyint(3) unsigned DEFAULT '0' COMMENT 'number of input rows to display on freetext',
  PRIMARY KEY (`question_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='questions for questionnaires: surveys, applications, etc';

DROP TABLE IF EXISTS `questionnaire`;
CREATE TABLE  `questionnaire` (
  `questionnaire_key` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL COMMENT 'title of the individiual questionnaire: e.g., Course 312 Evaluation',
  `class_key` int(10) unsigned NOT NULL COMMENT 'references dataitem_class table',
  PRIMARY KEY (`questionnaire_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='links a questionnaire to its container class';

DROP TABLE IF EXISTS `site`;
CREATE TABLE  `site` (
  `site_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `site_name` varchar(200) NOT NULL,
  `domain` varchar(200) NOT NULL DEFAULT '',
  `enabled` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `description` varchar(300) NOT NULL DEFAULT '',
  `default_template` varchar(200) NOT NULL,
  `print_template` varchar(200) DEFAULT NULL,
  `popup_template` varchar(200) DEFAULT NULL,
  `mobile_template` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`site_id`,`description`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

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

DROP VIEW IF EXISTS `forum_summary`;
CREATE VIEW `forum_summary` AS select `f`.`forum_id` AS `forum_id`,`f`.`title` AS `title`,`f`.`teaser` AS `teaser`,`f`.`published` AS `published`,(select count(1) AS `count(1)` from `forum_topic` where (`forum_topic`.`forum_id` = `f`.`forum_id`)) AS `topics`,(select count(1) AS `count(1)` from `forum_message` where ((`forum_message`.`forum_id` = `f`.`forum_id`) and (`forum_message`.`deleted` = 0))) AS `posts` from `forum` `f`;