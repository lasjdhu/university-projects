"""
Project: ITU
Author: Dmitrii Ivanushkin
File: SQLAlchemy ORM models.
Defines the database structure, tables, and relationships for the application
"""

from sqlalchemy import Column, Integer, String, Float, ForeignKey, Table, DateTime
from sqlalchemy.orm import relationship
from database import Base
from datetime import datetime


class DeviceGroup(Base):
    __tablename__ = "device_groups"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, nullable=False, index=True)

    devices = relationship(
        "Device", back_populates="group", cascade="all, delete-orphan"
    )


class Device(Base):
    __tablename__ = "devices"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    access_token = Column(String, nullable=False)

    group_id = Column(Integer, ForeignKey("device_groups.id"), nullable=False)
    group = relationship("DeviceGroup", back_populates="devices")

    commands = relationship(
        "Command", back_populates="device", cascade="all, delete-orphan"
    )
    widgets = relationship(
        "Widget", back_populates="device", cascade="all, delete-orphan"
    )
    topics = relationship(
        "DeviceTopic", back_populates="device", cascade="all, delete-orphan"
    )


class DeviceTopic(Base):
    __tablename__ = "device_topics"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    value = Column(Float, default=0.0)
    value_desc = Column(String, nullable=False, default="")

    device_id = Column(Integer, ForeignKey("devices.id"), nullable=False)
    device = relationship("Device", back_populates="topics")
    commands = relationship(
        "Command", back_populates="topic", cascade="all, delete-orphan"
    )
    widgets = relationship(
        "Widget", back_populates="topic", cascade="all, delete-orphan"
    )

    history = relationship(
        "DeviceTopicValue",
        back_populates="topic",
        order_by="DeviceTopicValue.timestamp",
        cascade="all, delete-orphan",
    )


class DeviceTopicValue(Base):
    __tablename__ = "device_topic_values"
    id = Column(Integer, primary_key=True, index=True)
    topic_id = Column(Integer, ForeignKey("device_topics.id"), nullable=False)
    value = Column(Float, nullable=False)
    timestamp = Column(DateTime, default=datetime.utcnow, nullable=False)

    topic = relationship("DeviceTopic", back_populates="history")


class Command(Base):
    __tablename__ = "commands"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    topic_id = Column(Integer, ForeignKey("device_topics.id"), nullable=False)
    value = Column(Float, nullable=True)
    device_id = Column(Integer, ForeignKey("devices.id"), nullable=False)

    device = relationship("Device", back_populates="commands")
    topic = relationship("DeviceTopic")


class WidgetGroup(Base):
    __tablename__ = "widget_groups"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, nullable=False)

    widgets = relationship(
        "Widget", back_populates="group", cascade="all, delete-orphan"
    )


class Widget(Base):
    __tablename__ = "widgets"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    icon = Column(String, nullable=False, default="?")
    color = Column(String, nullable=False, default="#888888")

    device_id = Column(Integer, ForeignKey("devices.id"), nullable=True)
    topic_id = Column(Integer, ForeignKey("device_topics.id"), nullable=True)
    group_id = Column(Integer, ForeignKey("widget_groups.id"), nullable=False)

    last_value = Column(Float, nullable=True)
    value_desc = Column(String, nullable=True)
    polling_interval = Column(Integer, nullable=True, default=5000)

    device = relationship("Device", back_populates="widgets")
    group = relationship("WidgetGroup", back_populates="widgets")
    topic = relationship("DeviceTopic", back_populates="widgets")
