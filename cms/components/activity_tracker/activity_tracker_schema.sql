-- START Version 1.0

DROP TABLE IF EXISTS `user_activity`;
CREATE TABLE  `user_activity` (
  `activity_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` varchar(45) NOT NULL DEFAULT '0',
  `uri` varchar(2000) DEFAULT NULL,
  `session_id` varchar(200) DEFAULT NULL,
  `method` varchar(5) NOT NULL,
  `referer` varchar(2000) DEFAULT NULL,
  `activity_time` datetime NOT NULL,
  PRIMARY KEY (`activity_id`)
) ENGINE=InnoDB;

ALTER TABLE `user_activity` ADD INDEX `activity_by_session`(`session_id`, `activity_time`);

DROP TABLE IF EXISTS `feedback`;
CREATE TABLE `feedback` (
  `feedback_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `session` varchar(200) NOT NULL,
  `feedback` text NOT NULL,
  `created_date` datetime NOT NULL,
  PRIMARY KEY (`feedback_id`)
) ENGINE=InnoDB;

CREATE OR REPLACE VIEW `activity_sessions` AS 
select `a`.`session_id` AS `session_id`, count(1) AS `page_views`,
min(`a`.`activity_time`) AS `session_start`,
max(`a`.`activity_time`) AS `session_end`,
`a`.`user_id` AS `user_id`,
(select count(1) AS `count(1)` from `feedback` where (`feedback`.`session` = `a`.`session_id`)) AS `num_feedback` from `user_activity` `a` group by `a`.`session_id`;


-- END Version 1.0

-- START Version 1.1

alter table `feedback` add column `referer` varchar(2000) DEFAULT NULL after `feedback`;

-- END Version 1.1

-- START Version 1.2

--ALTER TABLE user_activity ADD COLUMN response_time DECIMAL(10,3) NOT NULL DEFAULT 0.0;

-- END Version 1.2

-- START Version 1.3

CREATE TABLE user_activity_session (
	session_id	int(10) unsigned not null auto_increment,
	session_identifier varchar(200) not null,
	user_id	int(10) unsigned not null default 0,
	session_start datetime not null,
	session_end datetime not null,
	request_count int(10) unsigned not null default 0,
	feedback_count int(10) unsigned not null default 0,
	ip_address VARCHAR(40) NOT NULL DEFAULT '',
	user_agent VARCHAR(1000) NOT NULL DEFAULT '',
	action_count  int(10) unsigned not null default 0,
	page_views  int(10) unsigned not null default 0,
	PRIMARY KEY (session_id)
) ENGINE=InnoDB;

ALTER TABLE user_activity_session
ADD INDEX session_start_idx (session_start, feedback_count),
ADD INDEX session_user_idx (user_id, session_start, feedback_count);

-- END Version 1.3

