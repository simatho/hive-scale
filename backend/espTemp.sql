SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;


CREATE TABLE `devices` (
  `id` int(11) NOT NULL,
  `mac` varchar(17) NOT NULL,
  `chipid` varchar(20) NOT NULL,
  `ver` varchar(15) NOT NULL,
  `ver_upd` int(11) NOT NULL,
  `last_seen` int(11) NOT NULL,
  `follow_upstream` tinyint(1) NOT NULL DEFAULT '0',
  `class` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `device_class` (
  `id` int(11) NOT NULL,
  `name` varchar(25) COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `logins` (
  `id` int(11) NOT NULL,
  `user` varchar(20) COLLATE utf8_bin NOT NULL,
  `secret` varchar(255) COLLATE utf8_bin NOT NULL,
  `hash` varchar(255) COLLATE utf8_bin NOT NULL,
  `ip` varchar(255) COLLATE utf8_bin NOT NULL,
  `time_until` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `user` varchar(20) COLLATE utf8_bin NOT NULL,
  `hash` varchar(255) COLLATE utf8_bin NOT NULL,
  `time` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `versions` (
  `id` int(11) NOT NULL,
  `filename` varchar(31) COLLATE utf8_bin NOT NULL,
  `ver` varchar(15) COLLATE utf8_bin NOT NULL,
  `date` int(11) NOT NULL,
  `md5sum` varchar(32) COLLATE utf8_bin NOT NULL,
  `sha256sum` varchar(64) COLLATE utf8_bin NOT NULL,
  `class` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

CREATE TABLE `weightdata` (
  `id` bigint(20) UNSIGNED NOT NULL COMMENT 'Schlüssel',
  `temp_ext` float DEFAULT NULL COMMENT '°C',
  `weight` float UNSIGNED DEFAULT NULL COMMENT 'kg',
  `sensetime` bigint(20) UNSIGNED DEFAULT NULL,
  `chipmillis` bigint(20) UNSIGNED DEFAULT NULL,
  `chipid` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `sensorid_ext` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `servertime` bigint(20) UNSIGNED NOT NULL,
  `temp_int` float DEFAULT NULL COMMENT '°C',
  `vcc` float NOT NULL,
  `ver` text COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;


ALTER TABLE `devices`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `mac` (`mac`);

ALTER TABLE `device_class`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `name` (`name`);

ALTER TABLE `logins`
  ADD PRIMARY KEY (`id`);

ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `user` (`user`);

ALTER TABLE `versions`
  ADD PRIMARY KEY (`id`);

ALTER TABLE `weightdata`
  ADD PRIMARY KEY (`id`);


ALTER TABLE `devices`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
ALTER TABLE `device_class`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;
ALTER TABLE `logins`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=35;
ALTER TABLE `users`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
ALTER TABLE `versions`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
ALTER TABLE `weightdata`
  MODIFY `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Schlüssel', AUTO_INCREMENT=39;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
