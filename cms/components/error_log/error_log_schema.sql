-- Fakoli Error Log Component Schema
-- 
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `error_log` (
  `error_log_id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `code` INTEGER UNSIGNED NOT NULL,
  `message` TEXT NOT NULL,
  `file` VARCHAR(500) NOT NULL,
  `line` INTEGER UNSIGNED NOT NULL,
  `details` TEXT NOT NULL,
  `user_id` INTEGER UNSIGNED NOT NULL,
  `uri` VARCHAR(1000) NOT NULL DEFAULT '',
  `session` VARCHAR(200) NOT NULL DEFAULT '',
  `timestamp` DATETIME NOT NULL,
  PRIMARY KEY (`error_log_id`)
)
ENGINE = InnoDB;

-- END Version 1.0