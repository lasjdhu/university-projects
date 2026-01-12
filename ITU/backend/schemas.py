"""
Project: ITU
Author: Dmitrii Ivanushkin
File: Pydantic schemas for request/response validation.
Defines Data Transfer Objects.
"""

from pydantic import BaseModel
from typing import Optional, List
from datetime import datetime


class DeviceGroupBaseSchema(BaseModel):
    name: str

    class Config:
        from_attributes = True


class DeviceGroupCreateSchema(DeviceGroupBaseSchema):
    pass


class DeviceGroupReadSchema(DeviceGroupBaseSchema):
    id: int


class DeviceBaseSchema(BaseModel):
    name: str
    access_token: str

    class Config:
        from_attributes = True


class DeviceCreateSchema(DeviceBaseSchema):
    group_id: Optional[int] = None
    topic_names: Optional[List[str]] = None


class DeviceUpdateSchema(BaseModel):
    name: Optional[str] = None
    access_token: Optional[str] = None
    group_id: Optional[int] = None

    class Config:
        from_attributes = True


class DeviceReadSchema(DeviceBaseSchema):
    id: int
    group: DeviceGroupReadSchema


class DeviceTopicBaseSchema(BaseModel):
    name: str
    value: Optional[float] = 0.0
    device_id: int
    value_desc: Optional[str] = ""

    class Config:
        from_attributes = True


class DeviceTopicCreateSchema(DeviceTopicBaseSchema):
    pass


class DeviceTopicUpdateSchema(BaseModel):
    name: Optional[str] = None
    value: Optional[float] = None
    value_desc: Optional[str] = None

    class Config:
        from_attributes = True


class DeviceTopicReadSchema(DeviceTopicBaseSchema):
    id: int


class DeviceTopicValueReadSchema(BaseModel):
    id: int
    topic_id: int
    value: float
    timestamp: datetime

    class Config:
        from_attributes = True


class CommandBaseSchema(BaseModel):
    name: str
    topic_id: int
    device_id: int
    value: Optional[float] = None

    class Config:
        from_attributes = True


class CommandCreateSchema(CommandBaseSchema):
    pass


class CommandUpdateSchema(BaseModel):
    name: Optional[str] = None
    topic_id: Optional[int] = None
    device_id: Optional[int] = None
    value: Optional[float] = None

    class Config:
        from_attributes = True


class CommandReadSchema(CommandBaseSchema):
    id: int


class WidgetGroupBaseSchema(BaseModel):
    name: str

    class Config:
        from_attributes = True


class WidgetGroupCreateSchema(WidgetGroupBaseSchema):
    pass


class WidgetGroupReadSchema(WidgetGroupBaseSchema):
    id: int


class WidgetBaseSchema(BaseModel):
    name: str
    icon: Optional[str] = "?"
    color: Optional[str] = "#888888"
    device_id: Optional[int] = None
    topic_id: Optional[int] = None
    group_id: Optional[int] = None
    value_desc: Optional[str] = None
    polling_interval: Optional[int] = 5000

    class Config:
        from_attributes = True


class WidgetCreateSchema(WidgetBaseSchema):
    pass


class WidgetUpdateSchema(BaseModel):
    name: Optional[str] = None
    icon: Optional[str] = None
    color: Optional[str] = None
    device_id: Optional[int] = None
    topic_id: Optional[int] = None
    group_id: Optional[int] = None
    value_desc: Optional[str] = None
    polling_interval: Optional[int] = None

    class Config:
        from_attributes = True


class WidgetReadSchema(WidgetBaseSchema):
    id: int
    last_value: Optional[float] = None
