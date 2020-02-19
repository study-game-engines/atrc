#pragma once

#include <agz/editor/envir_light/envir_light.h>
#include <agz/editor/texture2d/texture2d.h>

AGZ_EDITOR_BEGIN

class IBLWidget : public EnvirLightWidget
{
    Q_OBJECT

public:

    IBLWidget(Texture2DSlot *tex, ObjectContext &obj_ctx);

    ResourceWidget<tracer::EnvirLight> *clone() override;

    QPixmap get_thumbnail(int width, int height) const override;

protected:

    void update_tracer_object_impl() override;

private:

    void do_update_tracer_object();

    Texture2DSlot *tex_     = nullptr;
    QCheckBox     *importance_sampling_ = nullptr;

    ObjectContext &obj_ctx_;
};

class IBLCreator : public EnvirLightWidgetCreator
{
public:

    QString name() const override { return "IBL"; }

    ResourceWidget<tracer::EnvirLight> *create_widget(ObjectContext &obj_ctx) const override;
};

AGZ_EDITOR_END
