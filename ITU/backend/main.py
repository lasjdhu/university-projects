"""
Project: ITU
Author: Dmitrii Ivanushkin
File: Main FastAPI application entry point.
Contains all API routes, dependency injection, and a background task
to simulate device data updates.
"""

import asyncio
import random

from fastapi import FastAPI, Depends, HTTPException, Query, Body
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy.orm import Session
from typing import List, Optional
import asyncio, random

from database import Base, engine, get_db, SessionLocal
from models import (
    DeviceGroup,
    Device,
    WidgetGroup,
    Widget,
    Command,
    DeviceTopic,
    DeviceTopicValue,
)
from schemas import (
    DeviceGroupReadSchema,
    DeviceGroupCreateSchema,
    DeviceReadSchema,
    DeviceCreateSchema,
    DeviceUpdateSchema,
    DeviceTopicReadSchema,
    DeviceTopicCreateSchema,
    DeviceTopicUpdateSchema,
    CommandReadSchema,
    CommandCreateSchema,
    CommandUpdateSchema,
    WidgetGroupReadSchema,
    WidgetGroupCreateSchema,
    WidgetReadSchema,
    WidgetCreateSchema,
    WidgetUpdateSchema,
)
import models, schemas


models.Base.metadata.create_all(bind=engine)

app = FastAPI(title="ITU")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


def get_or_create_device_all_group(db: Session) -> DeviceGroup:
    grp = db.query(DeviceGroup).filter_by(name="all").first()
    if not grp:
        grp = DeviceGroup(name="all")
        db.add(grp)
        db.commit()
        db.refresh(grp)
    return grp


def get_or_create_widget_all_group(db: Session) -> WidgetGroup:
    grp = db.query(WidgetGroup).filter_by(name="all").first()
    if not grp:
        grp = WidgetGroup(name="all")
        db.add(grp)
        db.commit()
        db.refresh(grp)
    return grp


def get_obj_or_404(db: Session, model, obj_id: int):
    obj = db.get(model, obj_id)
    if not obj:
        raise HTTPException(status_code=404, detail=f"{model.__name__} not found")
    return obj


async def simulate_device_topics():
    while True:
        db = SessionLocal()
        try:
            topics = db.query(DeviceTopic).all()
            for topic in topics:
                new_value = max(
                    0.0, min(100.0, (topic.value or 0.0) + random.uniform(-5, 5))
                )
                topic.value = new_value
                db.add(DeviceTopicValue(topic_id=topic.id, value=new_value))

                history = (
                    db.query(DeviceTopicValue)
                    .filter(DeviceTopicValue.topic_id == topic.id)
                    .order_by(DeviceTopicValue.timestamp.desc())
                    .all()
                )
                if len(history) > 50:
                    for old in history[50:]:
                        db.delete(old)
            db.commit()
        except Exception:
            db.rollback()
        finally:
            db.close()
        await asyncio.sleep(10)


@app.on_event("startup")
async def startup_event():
    asyncio.create_task(simulate_device_topics())


@app.post("/device-groups/", response_model=DeviceGroupReadSchema)
def create_device_group(item: DeviceGroupCreateSchema, db: Session = Depends(get_db)):
    exists = db.query(DeviceGroup).filter_by(name=item.name).first()
    # for time to be
    # if exists:
    #    raise HTTPException(status_code=400, detail="DeviceGroup with that name already exists")
    obj = DeviceGroup(name=item.name)
    db.add(obj)
    db.commit()
    db.refresh(obj)
    return obj


@app.get("/device-groups/", response_model=List[DeviceGroupReadSchema])
def list_device_groups(db: Session = Depends(get_db)):
    return db.query(DeviceGroup).order_by(DeviceGroup.id).all()


@app.get("/device-groups/{group_id}", response_model=DeviceGroupReadSchema)
def get_device_group(group_id: int, db: Session = Depends(get_db)):
    return get_obj_or_404(db, DeviceGroup, group_id)


@app.put("/device-groups/{group_id}", response_model=DeviceGroupReadSchema)
def update_device_group(
    group_id: int, item: DeviceGroupCreateSchema, db: Session = Depends(get_db)
):
    obj = get_obj_or_404(db, DeviceGroup, group_id)
    obj.name = item.name
    db.commit()
    db.refresh(obj)
    return obj


@app.delete("/device-groups/{group_id}")
def delete_device_group(group_id: int, db: Session = Depends(get_db)):
    obj = get_obj_or_404(db, DeviceGroup, group_id)
    db.delete(obj)
    db.commit()
    return {"detail": "deleted"}


@app.post("/devices/", response_model=DeviceReadSchema)
def create_device(item: DeviceCreateSchema, db: Session = Depends(get_db)):
    if item.group_id is None:
        grp = get_or_create_device_all_group(db)
        group_id = grp.id
    else:
        group = db.get(DeviceGroup, item.group_id)
        if not group:
            raise HTTPException(status_code=404, detail="group not found")
        group_id = group.id

    device = Device(name=item.name, access_token=item.access_token, group_id=group_id)
    db.add(device)
    db.commit()
    db.refresh(device)

    if item.topic_names:
        for tname in item.topic_names:
            topic = DeviceTopic(name=tname, value=0.0, device_id=device.id)
            db.add(topic)
        db.commit()
    db.refresh(device)
    return device


@app.get("/devices/", response_model=List[DeviceReadSchema])
def list_devices(group_id: Optional[int] = Query(None), db: Session = Depends(get_db)):
    q = db.query(Device)
    if group_id is not None:
        q = q.filter(Device.group_id == group_id)
    return q.order_by(Device.id).all()


@app.get("/devices/{device_id}", response_model=DeviceReadSchema)
def get_device(device_id: int, db: Session = Depends(get_db)):
    return get_obj_or_404(db, Device, device_id)


@app.put("/devices/{device_id}", response_model=DeviceReadSchema)
def update_device(
    device_id: int, item: DeviceUpdateSchema, db: Session = Depends(get_db)
):
    obj = get_obj_or_404(db, Device, device_id)
    if item.name is not None:
        obj.name = item.name
    if item.access_token is not None:
        obj.access_token = item.access_token
    if item.group_id is not None:
        grp = db.get(DeviceGroup, item.group_id)
        if not grp:
            raise HTTPException(status_code=404, detail="group not found")
        obj.group = grp
    db.commit()
    db.refresh(obj)
    return obj


@app.delete("/devices/{device_id}")
def delete_device(device_id: int, db: Session = Depends(get_db)):
    obj = get_obj_or_404(db, Device, device_id)
    db.delete(obj)
    db.commit()
    return {"detail": "deleted"}


@app.get("/device-topics/{device_id}", response_model=List[DeviceTopicReadSchema])
def list_device_topics(device_id: int, db: Session = Depends(get_db)):
    _ = get_obj_or_404(db, Device, device_id)
    return db.query(DeviceTopic).filter(DeviceTopic.device_id == device_id).all()


@app.post("/device-topics/{device_id}", response_model=DeviceTopicCreateSchema)
def create_device_topic(
    device_id: int, item: DeviceTopicCreateSchema, db: Session = Depends(get_db)
):
    _ = get_obj_or_404(db, Device, device_id)
    topic = DeviceTopic(
        name=item.name,
        value=item.value or 0.0,
        device_id=device_id,
        value_desc=item.value_desc or "",
    )
    db.add(topic)
    db.commit()
    db.refresh(topic)
    return topic


@app.put("/device-topics/{device_id}/{topic_id}", response_model=DeviceTopicReadSchema)
def update_device_topic(
    device_id: int,
    topic_id: int,
    item: DeviceTopicUpdateSchema,
    db: Session = Depends(get_db),
):
    _ = get_obj_or_404(db, Device, device_id)
    topic = get_obj_or_404(db, DeviceTopic, topic_id)
    if topic.device_id != device_id:
        raise HTTPException(
            status_code=400, detail="Topic does not belong to this device"
        )
    if item.name is not None:
        topic.name = item.name
    if item.value is not None:
        topic.value = item.value
        db.add(DeviceTopicValue(topic_id=topic.id, value=topic.value))
    if item.value_desc is not None:
        topic.value_desc = item.value_desc
    db.commit()
    db.refresh(topic)
    return topic


@app.delete("/device-topics/{device_id}/{topic_id}")
def delete_device_topic(device_id: int, topic_id: int, db: Session = Depends(get_db)):
    _ = get_obj_or_404(db, Device, device_id)
    topic = get_obj_or_404(db, DeviceTopic, topic_id)
    if topic.device_id != device_id:
        raise HTTPException(
            status_code=400, detail="Topic does not belong to this device"
        )
    db.delete(topic)
    db.commit()
    return {"detail": "deleted"}


@app.get(
    "/device-topics/device/{device_id}", response_model=List[DeviceTopicReadSchema]
)
def list_device_topics_for_widget(device_id: int, db: Session = Depends(get_db)):
    return list_device_topics(device_id, db)


@app.get("/devices/{device_id}/commands/", response_model=List[CommandReadSchema])
def list_commands_for_device(device_id: int, db: Session = Depends(get_db)):
    _ = get_obj_or_404(db, Device, device_id)
    return db.query(Command).filter(Command.device_id == device_id).all()


@app.post("/devices/{device_id}/commands/", response_model=CommandReadSchema)
def create_command_for_device(
    device_id: int, item: CommandCreateSchema, db: Session = Depends(get_db)
):
    _ = get_obj_or_404(db, Device, device_id)
    topic = db.get(DeviceTopic, item.topic_id)
    if not topic:
        raise HTTPException(status_code=404, detail="topic not found")
    if topic.device_id != device_id:
        raise HTTPException(status_code=400, detail="topic does not belong to device")
    cmd = Command(
        name=item.name, topic_id=item.topic_id, value=item.value, device_id=device_id
    )
    db.add(cmd)
    db.commit()
    db.refresh(cmd)
    return cmd


@app.put("/devices/{device_id}/commands/{command_id}", response_model=CommandReadSchema)
def update_command(
    device_id: int,
    command_id: int,
    item: CommandUpdateSchema,
    db: Session = Depends(get_db),
):
    _ = get_obj_or_404(db, Device, device_id)
    cmd = get_obj_or_404(db, Command, command_id)
    if cmd.device_id != device_id:
        raise HTTPException(
            status_code=400, detail="Command does not belong to this device"
        )
    if item.name is not None:
        cmd.name = item.name
    if item.value is not None:
        cmd.value = item.value
    if item.topic_id is not None:
        topic = db.get(DeviceTopic, item.topic_id)
        if not topic:
            raise HTTPException(status_code=404, detail="topic not found")
        if topic.device_id != device_id:
            raise HTTPException(
                status_code=400, detail="topic does not belong to device"
            )
        cmd.topic_id = item.topic_id
    db.commit()
    db.refresh(cmd)
    return cmd


@app.delete("/commands/{command_id}")
def delete_command(command_id: int, db: Session = Depends(get_db)):
    cmd = get_obj_or_404(db, Command, command_id)
    db.delete(cmd)
    db.commit()
    return {"detail": "deleted"}


@app.post("/commands/{command_id}/run")
def run_command_by_id(command_id: int, db: Session = Depends(get_db)):
    cmd = get_obj_or_404(db, Command, command_id)
    topic = db.get(DeviceTopic, cmd.topic_id)
    if not topic:
        raise HTTPException(status_code=404, detail="topic not found")
    if cmd.value is None:
        raise HTTPException(status_code=400, detail="Command has no value to apply")
    topic.value = cmd.value
    db.add(DeviceTopicValue(topic_id=topic.id, value=topic.value))
    db.commit()
    return {
        "status": "ok",
        "message": f"Executed command '{cmd.name}'",
        "value": topic.value,
    }


@app.post("/widget-groups/", response_model=WidgetGroupReadSchema)
def create_widget_group(item: WidgetGroupCreateSchema, db: Session = Depends(get_db)):
    exists = db.query(WidgetGroup).filter_by(name=item.name).first()
    if exists:
        raise HTTPException(
            status_code=400, detail="WidgetGroup with that name already exists"
        )
    obj = WidgetGroup(name=item.name)
    db.add(obj)
    db.commit()
    db.refresh(obj)
    return obj


@app.get("/widget-groups/", response_model=List[WidgetGroupReadSchema])
def list_widget_groups(db: Session = Depends(get_db)):
    return db.query(WidgetGroup).order_by(WidgetGroup.id).all()


@app.get("/widget-groups/{group_id}", response_model=WidgetGroupReadSchema)
def get_widget_group(group_id: int, db: Session = Depends(get_db)):
    return get_obj_or_404(db, WidgetGroup, group_id)


@app.put("/widget-groups/{group_id}", response_model=WidgetGroupReadSchema)
def update_widget_group(
    group_id: int, item: WidgetGroupCreateSchema, db: Session = Depends(get_db)
):
    obj = get_obj_or_404(db, WidgetGroup, group_id)
    obj.name = item.name
    db.commit()
    db.refresh(obj)
    return obj


@app.delete("/widget-groups/{group_id}")
def delete_widget_group(group_id: int, db: Session = Depends(get_db)):
    obj = get_obj_or_404(db, WidgetGroup, group_id)
    db.delete(obj)
    db.commit()
    return {"detail": "deleted"}


@app.post("/widgets/", response_model=WidgetReadSchema)
def create_widget(item: WidgetCreateSchema, db: Session = Depends(get_db)):
    if item.group_id is None:
        grp = get_or_create_widget_all_group(db)
        group_id = grp.id
    else:
        grp = db.get(WidgetGroup, item.group_id)
        if not grp:
            raise HTTPException(status_code=404, detail="widget group not found")
        group_id = grp.id

    widget = Widget(
        name=item.name,
        icon=item.icon or "?",
        color=item.color or "#888888",
        device_id=item.device_id,
        topic_id=item.topic_id,
        group_id=group_id,
        value_desc=item.value_desc,
        polling_interval=item.polling_interval,
    )
    if widget.device_id is not None:
        if not db.get(Device, widget.device_id):
            raise HTTPException(status_code=404, detail="device not found")
    if widget.topic_id is not None:
        if not db.get(DeviceTopic, widget.topic_id):
            raise HTTPException(status_code=404, detail="topic not found")

    db.add(widget)
    db.commit()
    db.refresh(widget)
    return widget


@app.get("/widgets/", response_model=List[WidgetReadSchema])
def list_widgets(group_id: Optional[int] = Query(None), db: Session = Depends(get_db)):
    q = db.query(Widget)
    if group_id is not None:
        q = q.filter(Widget.group_id == group_id)
    return q.order_by(Widget.id).all()


@app.get("/widgets/{widget_id}", response_model=WidgetReadSchema)
def get_widget(widget_id: int, db: Session = Depends(get_db)):
    return get_obj_or_404(db, Widget, widget_id)


@app.patch("/widgets/{widget_id}", response_model=WidgetReadSchema)
def patch_widget(
    widget_id: int, item: WidgetUpdateSchema, db: Session = Depends(get_db)
):
    widget = get_obj_or_404(db, Widget, widget_id)
    data = item.dict(exclude_unset=True)
    if "group_id" in data:
        grp = db.get(WidgetGroup, data["group_id"])
        if not grp:
            raise HTTPException(status_code=404, detail="widget group not found")
    if "device_id" in data and data["device_id"] is not None:
        if not db.get(Device, data["device_id"]):
            raise HTTPException(status_code=404, detail="device not found")
    if "topic_id" in data and data["topic_id"] is not None:
        if not db.get(DeviceTopic, data["topic_id"]):
            raise HTTPException(status_code=404, detail="topic not found")

    for k, v in data.items():
        setattr(widget, k, v)
    db.commit()
    db.refresh(widget)
    return widget


@app.delete("/widgets/{widget_id}")
def delete_widget(widget_id: int, db: Session = Depends(get_db)):
    obj = get_obj_or_404(db, Widget, widget_id)
    db.delete(obj)
    db.commit()
    return {"detail": "deleted"}


@app.get("/widgets/{widget_id}/data")
def get_widget_data(widget_id: int, db: Session = Depends(get_db)):
    widget = get_obj_or_404(db, Widget, widget_id)
    if widget.topic_id is None:
        raise HTTPException(status_code=404, detail="Widget has no associated topic")
    topic = get_obj_or_404(db, DeviceTopic, widget.topic_id)

    history = (
        db.query(DeviceTopicValue)
        .filter(DeviceTopicValue.topic_id == topic.id)
        .order_by(DeviceTopicValue.timestamp.desc())
        .limit(50)
        .all()
    )
    widget.last_value = topic.value
    db.commit()
    db.refresh(widget)

    return {
        "widget_id": widget.id,
        "topic": topic.name,
        "last_value": widget.last_value,
        "history": [
            {"value": v.value, "timestamp": v.timestamp.isoformat()}
            for v in reversed(history)
        ],
    }


@app.post("/widgets/{widget_id}/run/{command_id}")
def run_command_from_widget(
    widget_id: int, command_id: int, db: Session = Depends(get_db)
):
    widget = get_obj_or_404(db, Widget, widget_id)
    cmd = get_obj_or_404(db, Command, command_id)

    if widget.device_id != cmd.device_id or widget.topic_id != cmd.topic_id:
        raise HTTPException(
            status_code=400,
            detail="Command not associated with this widget's topic/device",
        )
    if cmd.value is None:
        raise HTTPException(status_code=400, detail="Command has no value to apply")

    topic = get_obj_or_404(db, DeviceTopic, widget.topic_id)
    topic.value = cmd.value
    db.add(DeviceTopicValue(topic_id=topic.id, value=topic.value))
    db.commit()
    return {
        "status": "ok",
        "message": f"Executed command '{cmd.name}' on topic '{topic.name}'",
        "value": topic.value,
    }


@app.get("/")
def root():
    return {"ok": True, "service": "ITU is running"}
