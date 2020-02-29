#include <QCheckBox>

#include <agz/editor/texture3d/constant3d.h>

AGZ_EDITOR_BEGIN

Constant3DWidget::Constant3DWidget(const InitData &init_data)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    input_color_     = new SpectrumInput(this);
    use_input_color_ = new QCheckBox("Use Input Color", this);
    color_holder_    = new ColorHolder(init_data.color_holder_value, this);

    use_input_color_->setChecked(init_data.use_input_color);
    input_color_->set_alignment(Qt::AlignCenter);
    input_color_->set_value({
        init_data.input_value.r,
        init_data.input_value.g,
        init_data.input_value.b });

    layout->addWidget(use_input_color_);
    layout->addWidget(input_color_);
    layout->addWidget(color_holder_);

    if(init_data.use_input_color)
        color_holder_->hide();
    else
        input_color_->hide();

    setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(use_input_color_, &QCheckBox::stateChanged, [=](int)
    {
        if(use_input_color_->isChecked())
        {
            color_holder_->hide();
            input_color_->show();
        }
        else
        {
            input_color_->hide();
            color_holder_->show();
        }

        set_dirty_flag();
    });

    connect(input_color_, &SpectrumInput::edit_value, [=](const Spectrum&)
    {
        set_dirty_flag();
    });

    connect(color_holder_, &ColorHolder::change_color, [=](const Spectrum &)
    {
        set_dirty_flag();
    });

    if(use_input_color_->isChecked())
        tracer_object_ = tracer::create_constant3d_texture({}, input_color_->get_value());
    else
        tracer_object_ = tracer::create_constant3d_texture({}, color_holder_->get_color());
}

Texture3DWidget *Constant3DWidget::clone()
{
    InitData init_data;
    init_data.use_input_color    = use_input_color_->isChecked();
    init_data.input_value        = input_color_->get_value();
    init_data.color_holder_value = color_holder_->get_color();

    return new Constant3DWidget(init_data);
}

std::unique_ptr<ResourceThumbnailProvider> Constant3DWidget::get_thumbnail(int width, int height) const
{
    QImage image(1, 1, QImage::Format::Format_RGB888);
    image.setPixelColor(0, 0, color_holder_->get_qcolor());

    QPixmap pixmap;
    pixmap.convertFromImage(image);

    return std::make_unique<FixedResourceThumbnailProvider>(pixmap.scaled(width, height));
}

void Constant3DWidget::save_asset(AssetSaver &saver)
{
    saver.write(uint8_t(use_input_color_->isChecked() ? 1 : 0));
    saver.write(color_holder_->get_color());
    saver.write(input_color_->get_value());
}

void Constant3DWidget::load_asset(AssetLoader &loader)
{
    use_input_color_->setChecked(loader.read<uint8_t>() != 0);
    color_holder_->set_color(loader.read<Spectrum>());
    input_color_->set_value(loader.read<Spectrum>());

    if(use_input_color_->isChecked())
        tracer_object_ = tracer::create_constant3d_texture({}, input_color_->get_value());
    else
        tracer_object_ = tracer::create_constant3d_texture({}, color_holder_->get_color());
}

void Constant3DWidget::update_tracer_object_impl()
{
    if(use_input_color_->isChecked())
        tracer_object_ = tracer::create_constant3d_texture({}, input_color_->get_value());
    else
        tracer_object_ = tracer::create_constant3d_texture({}, color_holder_->get_color());
}

ResourceWidget<tracer::Texture3D> *Constant3DWidgetCreator::create_widget(ObjectContext &obj_ctx) const
{
    return new Constant3DWidget({});
}

AGZ_EDITOR_END