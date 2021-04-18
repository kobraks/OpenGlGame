Scene = 
{
    Title = "Untilted";
    
    Entities = 
    {
        {
            Id = 1;

            TagComponent = {
                Tag = "Cow"
            };

            TransformComponent = 
            {
                --Optionaly Position = { x = 0; y = 0; z= 0 };
                Position = { 0, 0, 0 };
                Scale = { 0, 0, 0};
                Rotation = { 0, 0, 0 };
            };

            ModelComponent = 
            {
                Drawable = true; --optional in case of true
                Path = "Cow.obj"
            };
        };
        {
            TagComponent = {
                Tag = "Camera"
            };

            TransformComponent = 
            {
                Position = { 0, 0, 0 };
                Scale = { 0, 0, 0};
                Rotation = { 0, 0, 1 };
            };

            CameraComponent = 
            {
                    
                Type = CameraType.Perspective;

                NearClip  = 0.001;
                FarClip = 1000;
                FOV = 45; --in case of Perspective camera type
                Size = 10; --Ignored if not Orthographic camera type


                Primary = true;
                FixedAspectRatio = false;

                --Ignored in case of FixedAspectRatio = false
                AspectRatio = 800 / 600;
            };
        };
        {
            TagComponent = {
                Tag = "DirectionalLight";
            };

            TransformComponent = {
                --Ignored in case of directional light
                Position = {0, 0, 0};
                --Direction ignored if not Directional light
                Rotation = { 0, -1, 0};
            };

            LightComponent = 
            {
                Active = true; --Redundand in case of false

                Type = LightType.Directional;

                DiffuseColor = { 255, 255, 255};
                AmbientColor = { 255, 255, 255};
                SpecularColor = { 255, 255, 255};
        
                Constant = 0;
                Linear = 0;
                Quadratic = 0;
        
                --Ignored if not Spot light
                CutOff = 0;
                OuterCutOff = 0;

                --Optional spot light texture Cookie
                LightCookie = "flashlight.png";
            };
        };
    };
};