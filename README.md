# UnrealVision - Synthetic Dataset Generator for Pose Estimation and Behavior Analysis

UnrealVision (UVis) is a synthetic dataset generator developed to advance pose estimation and behavior analysis using highly realistic, labeled synthetic data. Leveraging Unreal Engine 5, UVis provides a customizable environment for generating annotated datasets essential for training computer vision models, specifically in human pose estimation and interaction analysis. This project facilitates synthetic data generation with built-in support for custom scenarios, multiple camera perspectives, and automated data labeling compatible with popular pose estimation frameworks.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Usage](#usage)
- [Dataset Generation](#dataset-generation)
- [Supported Formats](#supported-formats)
- [Evaluation](#evaluation)

## Overview
UnrealVision addresses the growing need for high-quality synthetic data for pose estimation and behavior analysis tasks in computer vision. Traditional datasets often lack the flexibility required for domain-specific scenarios, while manual annotation can be labor-intensive and error-prone. UVis mitigates these issues by automating the creation of realistic synthetic datasets with configurable environments, character behaviors, and detailed annotations.

## Features
- **High-Fidelity 3D Simulation Environment**: Utilizes Unreal Engine 5's Lumen lighting and rendering capabilities for realistic images.
- **Configurable Scenarios and Characters**: Generate diverse scenarios with adjustable human behaviors, including interactions and crowds.
- **Multi-View Camera Support**: Capture scenes from multiple angles with adjustable field of view, position, and rotation.
- **Automated Annotations**: Provides annotations for color images, depth maps, normal maps, and semantic segmentation. Keypoints are compatible with OpenPose and YOLO formats.
- **Real-Time Streaming**: Allows inference evaluation via real-time camera streaming.
- **Post-Processing Tools**: Includes Python scripts for data transformation and exporting annotations.

## Dataset Generation
UVis generates datasets with automatic labeling for training machine learning models. Datasets include:
- **2D and 3D Pose Annotations**: Generated according to OpenPose and YOLO formats.
- **Segmentation Maps**: Labels each object for semantic segmentation tasks.
- **Multi-View Images**: Captured at user-defined intervals across multiple camera angles.

## Supported Formats
The generator supports a variety of output formats:
- **Color Images**: RGB images with realistic lighting effects.
- **Depth Maps**: Grayscale maps indicating object distances.
- **Normal Maps**: Encodes surface orientation for detailed geometry analysis.
- **Segmentation Masks**: Identifies and differentiates object boundaries.

## Evaluation
The project includes performance metrics for models trained on UVis datasets, such as YOLOv11-Pose, to assess the impact of synthetic data on real-world inference. Evaluation tools for analyzing camera angles and model accuracy are provided.