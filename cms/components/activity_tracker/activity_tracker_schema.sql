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