# RFID-Vehicle-Return-System

Overview:

The Vehicle Return Notifier is a system written in Arduino IDE, that uses an NFC reader wired to an ESP32 to detect RFID tags associated with vehicles. When a tagged vehicle is scanned, the system compares the tag’s ID with predefined vehicle IDs and sends a notification via Telegram if a match is found, indicating that the vehicle has been returned.

Purpose:

This project aims to streamline the key return process for the van rental company. By installing this NFC-based system in the key return box, the business can automatically track the return of keys and know where they are, improving efficiency and reducing the need for manual monitoring.

Features
-NFC Tag Scanning: Detects RFID tags using the PN532 NFC reader.

-Vehicle ID Matching: Compares scanned RFID tags with predefined vehicle IDs.

-Telegram Notification: Sends notifications to a specified Telegram chat when a matching vehicle is returned.

